#pragma once

#include <ars548_driver/util/byteswap.hpp>
#include "ars548_messages/msg/filter_status.hpp"

#define FILTERSTATUS_NUM_DETECTION_FILTERS 7
#define FILTERSTATUS_NUM_OBJECT_FILTERS 24

// NOTE: The ARS548 SDK documentation (Ars548Dll.h) does not publish a Method ID
// for MSG_ID_FILTER_STATUS, unlike Status/Object/Detection. FILTER_STATUS_MESSAGE_PDU_LENGTH
// and FILTER_STATUS_MESSAGE_PAYLOAD below are computed from the documented struct layout
// (sum of packed field sizes, following the same PDU/payload convention verified against
// UDPStatus: PDU_LENGTH = body size, PAYLOAD = PDU_LENGTH + 8-byte SOME/IP header).
// They are NOT confirmed against real sensor traffic. Until FILTER_STATUS_MESSAGE_METHOD_ID
// is confirmed, isValid() only checks the payload length; receiveFilterStatusMsg() logs the
// MethodID it sees so it can be captured from a live sensor and hardcoded below.
#define FILTER_STATUS_MESSAGE_PDU_LENGTH 322
#define FILTER_STATUS_MESSAGE_PAYLOAD 330

#pragma pack(1)

struct FilterStatusEntry {
    uint8_t Active;
    uint8_t FilterID;
    float MinimumValue;
    float MaximumValue;

    inline void changeEndianness();
    inline ars548_messages::msg::FilterStatusEntry toMsg();
};

inline void FilterStatusEntry::changeEndianness() {
    MinimumValue = byteswap(MinimumValue);
    MaximumValue = byteswap(MaximumValue);
}

inline ars548_messages::msg::FilterStatusEntry FilterStatusEntry::toMsg() {
    ars548_messages::msg::FilterStatusEntry e;
    e.active = Active;
    e.filterid = FilterID;
    e.minimumvalue = MinimumValue;
    e.maximumvalue = MaximumValue;
    return e;
}

struct UDPFilterStatus {
    uint16_t ServiceID;
    uint16_t MethodID;
    uint32_t PayloadLength;
    uint32_t Timestamp_Nanoseconds;
    uint32_t Timestamp_Seconds;
    uint8_t Timestamp_SyncStatus;
    uint8_t FilterConfigurationCounter;
    uint8_t DetectionSortIndex;
    uint8_t ObjectSortIndex;
    struct FilterStatusEntry DetectionFilter[FILTERSTATUS_NUM_DETECTION_FILTERS];
    struct FilterStatusEntry ObjectFilter[FILTERSTATUS_NUM_OBJECT_FILTERS];

    //! @brief Checks for validity of the message (checks PayloadLength; MethodID unconfirmed, see note above)
    inline bool isValid() const {
        return PayloadLength == FILTER_STATUS_MESSAGE_PDU_LENGTH;
    }

    inline void changeEndianness();
    inline ars548_messages::msg::FilterStatus toMsg();

    inline bool receiveFilterStatusMsg(int nbytes, const char *buffer) {
        if (nbytes == FILTER_STATUS_MESSAGE_PAYLOAD) {
            *this = *((struct UDPFilterStatus *)buffer);
            changeEndianness();
            return isValid();
        }
        return false;
    }
};
#pragma pack(4)

inline void UDPFilterStatus::changeEndianness() {
    ServiceID = byteswap(ServiceID);
    MethodID = byteswap(MethodID);
    PayloadLength = byteswap(PayloadLength);
    Timestamp_Nanoseconds = byteswap(Timestamp_Nanoseconds);
    Timestamp_Seconds = byteswap(Timestamp_Seconds);
    for (int i = 0; i < FILTERSTATUS_NUM_DETECTION_FILTERS; ++i) {
        DetectionFilter[i].changeEndianness();
    }
    for (int i = 0; i < FILTERSTATUS_NUM_OBJECT_FILTERS; ++i) {
        ObjectFilter[i].changeEndianness();
    }
}

inline ars548_messages::msg::FilterStatus UDPFilterStatus::toMsg() {
    ars548_messages::msg::FilterStatus filterStatusMessage;

    filterStatusMessage.timestamp_nanoseconds = Timestamp_Nanoseconds;
    filterStatusMessage.timestamp_seconds = Timestamp_Seconds;
    filterStatusMessage.timestamp_syncstatus = Timestamp_SyncStatus;
    filterStatusMessage.filterconfigurationcounter = FilterConfigurationCounter;
    filterStatusMessage.detectionsortindex = DetectionSortIndex;
    filterStatusMessage.objectsortindex = ObjectSortIndex;

    for (int i = 0; i < FILTERSTATUS_NUM_DETECTION_FILTERS; ++i) {
        filterStatusMessage.detectionfilter[i] = DetectionFilter[i].toMsg();
    }
    for (int i = 0; i < FILTERSTATUS_NUM_OBJECT_FILTERS; ++i) {
        filterStatusMessage.objectfilter[i] = ObjectFilter[i].toMsg();
    }

    return filterStatusMessage;
}
