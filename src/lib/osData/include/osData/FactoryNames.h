// \brief	List all available factory names for IDataExchange

#pragma once

namespace NS_OSBASE::data {
    constexpr char IDATAEXCHANGE_WEBSOCKET_FACTORY_NAME[] = "osbase.data.idataexhange.WebSocketDataExchange";
    constexpr char IFILEEXCHANGE_LOCALFILE_FACTORY_NAME[] = "osbase.data..ifileexchange.LocalFileExchange";
    constexpr char WAMPCCBROCKER_FACTORY_NAME[]           = "osbase.data.ibroker.wampccbroker";
    constexpr char MESSAGINGWAMPCC_FACTORY_NAME[]         = "osbase.data.imessaging.messagingwampcc";
    constexpr char LOGOUTPUT_NULL_FACTORY_NAME[]          = "osbase.data.logoutput.null";
    constexpr char LOGOUTPUT_FILE_FACTORY_NAME[]          = "osbase.data.logoutput.file";
    constexpr char LOGOUTPUT_DATAEXCHANGE_FACTORY_NAME[]  = "osbase.data.logoutput.dataexchange";
    constexpr char LOGOUTPUT_CONSOLE_FACTORY_NAME[]       = "osbase.data.logoutput.console";
    constexpr char LOGOUTPUT_DEBUG_FACTORY_NAME[]         = "osbase.data.logoutput.debug";
    constexpr char NETWORK_FACTORY_NAME[]                 = "sb.shared.osbase.data.inetwork.network";
} // namespace NS_OSBASE::data
