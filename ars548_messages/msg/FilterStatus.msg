std_msgs/Header header
uint32 timestamp_nanoseconds #Timestamp Nanoseconds
uint32 timestamp_seconds #Timestamp Seconds
uint8 timestamp_syncstatus #Timestamp Sync Status
uint8 filterconfigurationcounter #Counter that counts up if new filter configuration has been received and accepted
uint8 detectionsortindex #Detection list sorting index
uint8 objectsortindex #Object list sorting index
FilterStatusEntry[7] detectionfilter #Detection filter entries (index 1 to 7)
FilterStatusEntry[24] objectfilter #Object filter entries (index 1 to 24)
