// Read the Microsoft Flight Simulator documentation.
// https://docs.flightsimulator.com/html/Programming_Tools/SimConnect/SimConnect_SDK.htm

#include "msfs_connect.h"

HANDLE simConnect = NULL;
bool quit = false;

struct Struct1
{
    char title[256];

    // Ambient/Weather SimVars.
    double pressure; // inHg
    double baro_pressure; // millibars
    double sea_pressure; // millibars
    double temp; // celsius
    double visibility; // meters
    double wind_dir; // degrees
    double wind_vlcty; // knots
};

enum EVENT_ID
{
    EVENT_SIM_START,
};

enum DATA_DEFINE_ID
{
    DEFINITION_1,
};

enum DATA_REQUEST_ID
{
    REQUEST_1,
};

SIMCONNECT_RECV* pData;
DWORD cbData;

void CALLBACK MSFSConnect::dispatchProc()
{
    switch (pData->dwID)
    {
    case SIMCONNECT_RECV_ID_EVENT:
    {
        SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT*)pData;
        if (evt->uEventID == EVENT_SIM_START) {
            // Get the informations about the user's aricraft.
            // The data will then be received on SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE.
            SimConnect_RequestDataOnSimObjectType(simConnect, REQUEST_1, DEFINITION_1, 0, SIMCONNECT_SIMOBJECT_TYPE_USER);
        }
        break;
    }
    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
    {
        SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*)pData;
        if (pObjData->dwRequestID != REQUEST_1) break; // Here we only have one request, so it's not really relevant.

        // Get all the data from MSFS and cast them to our Struct1.
        // Now we have our data!
        Struct1* pS = (Struct1*)&pObjData->dwData;

        // Check if the title length doesn't exceeds the stuct size limit (256).
        if (!SUCCEEDED(StringCbLengthA(&pS->title[0], sizeof(pS->title), NULL))) break;

        // Emit the received data to our main UI window.
        // There's probably a better way to do this.
        emit MSFSConnect::clearText();

        emit MSFSConnect::appendText(QString("Type: %1\n").arg(pS->title));
        emit MSFSConnect::appendText(QString("Pressure: %1 inHg").arg(pS->pressure));
        emit MSFSConnect::appendText(QString("Barometer Pressure: %1 mB").arg(pS->baro_pressure));
        emit MSFSConnect::appendText(QString("Sea Leavel Pressure: %1 mB").arg(pS->sea_pressure));
        emit MSFSConnect::appendText(QString("Temperature: %1°C").arg(pS->temp));
        emit MSFSConnect::appendText(QString("Visibility: %1 meters").arg(pS->visibility));
        emit MSFSConnect::appendText(QString("Wind Direction: %1°").arg(pS->wind_dir));
        emit MSFSConnect::appendText(QString("Wind Velocity: %1 knots").arg(pS->wind_vlcty));

        break;
    }
    // Received when the user leaves MSFS.
    case SIMCONNECT_RECV_ID_QUIT:
    {
        quit = true;
        break;
    }
    default:
        break;
    }
}

bool MSFSConnect::connect()
{
    qInfo("Connecting to SimConnect...");

    if (SUCCEEDED(SimConnect_Open(&simConnect, "MSFS NavData", NULL, 0, 0, 0)))
    {
        // Defining the SimVars we want to get, and set them to our DEFINITION_1.
        // See the documentation on Simulation Variables.
        // https://docs.flightsimulator.com/html/Programming_Tools/SimVars/Simulation_Variables.htm
        // https://docs.flightsimulator.com/html/Programming_Tools/SimConnect/API_Reference/Events_And_Data/SimConnect_AddToDataDefinition.htm
        SimConnect_AddToDataDefinition(simConnect, DEFINITION_1, "Title", NULL, SIMCONNECT_DATATYPE_STRING256);
        SimConnect_AddToDataDefinition(simConnect, DEFINITION_1, "Ambient Pressure", "inHg");
        SimConnect_AddToDataDefinition(simConnect, DEFINITION_1, "Barometer Pressure", "millibars");
        SimConnect_AddToDataDefinition(simConnect, DEFINITION_1, "Sea Level Pressure", "millibars");
        SimConnect_AddToDataDefinition(simConnect, DEFINITION_1, "Ambient Temperature", "celsius");
        SimConnect_AddToDataDefinition(simConnect, DEFINITION_1, "Ambient Visibility", "meters");
        SimConnect_AddToDataDefinition(simConnect, DEFINITION_1, "Ambient Wind Direction", "degrees");
        SimConnect_AddToDataDefinition(simConnect, DEFINITION_1, "Ambient Wind Velocity", "knots");

        // Subscribe to the SimConnect event to get data every second.
        // https://docs.flightsimulator.com/html/Programming_Tools/SimConnect/API_Reference/Events_And_Data/SimConnect_SubscribeToSystemEvent.htm
        SimConnect_SubscribeToSystemEvent(simConnect, EVENT_SIM_START, "1sec");

        qInfo("Connected to SimConnect!");

        return true;
    }
    else
    {
        qInfo("SimConnect connection failed!");
        return false;
    }
}

/**
 * Try to get the data from MSFS in a new thread.
 */
void MSFSConnect::loop()
{
    quit = 0;

    QThread *thread = QThread::create([this] {
        while (!quit) {
            // Process the data to the dispatcher.
            // SimConnect_CallDispatch could also be used.
            // https://docs.flightsimulator.com/html/Programming_Tools/SimConnect/API_Reference/General/SimConnect_GetNextDispatch.htm
            if(SUCCEEDED(SimConnect_GetNextDispatch(simConnect, &pData, &cbData))) {
                dispatchProc();
            }
            Sleep(1); // We can also use QTimer instead.
        }
    });

    thread->start();

    qInfo("Looping thread started!");
}

/**
 * Stop the thread loop, and disconnect from SimConnect.
 */
void MSFSConnect::disconnect()
{
    quit = true;
    if(simConnect != NULL) SimConnect_Close(simConnect);
    simConnect = NULL;
    emit MSFSConnect::clearText();
    emit MSFSConnect::appendText("No data.");
    qInfo("Disconnected from SimConnect!");
}

MSFSConnect::MSFSConnect(QObject *parent)
    : QObject{parent}
{

}

