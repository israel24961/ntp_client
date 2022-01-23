#include <sys/types.h>

#include <stdlib.h>
#include <stdint.h>

/**
 *  Important, the functions don't check whether the
 *  ntp packet pointer is null
 *
 **/

typedef struct
{//384 bits or 48 bytes
    uint8_t Li_Vn_Mode;
    uint8_t Stratum;
    int8_t Poll;       // Minimum interval between mesages 2**Poll[seconds], i.e. Poll=6-->64 seconds
    int8_t Precision;  //Precision of the various clocks, 2**Precision[seconds], i.e. -5-->31.25ms

    int32_t  Rootdelay_s;     // Total roundtrip delay to the primary reference src(in seconds).
    int32_t RootDispersion_s; // Maximum error relative to root
    uint32_t RefClkId;          //  String identifying the particular reference clock.(See Appendix A, RFC 1305)
//Timestamp format: 64bit unsigned fixed point
//                  integer in the first 32bits
//                  decimals seconds in the last 32bits
    uint32_t Ref_s;     //  Local time, when the clock was last updated. (Zero if never sync)
    uint32_t Ref_ds;    // decimals

    uint32_t Ori_s;       //  Local time, at peer when its latest NTP msg was sent. (Zero if unreachable)
    uint32_t Ori_ds;       //  decimals

    uint32_t Rx_s;         //  Local time, when the lates NTP message from the peer arrived. (Zero if unreachable)
    uint32_t Rx_ds;         //  decimals

    uint32_t Tx_s;         //  Local time, at which the NTP message departed the sender
    uint32_t Tx_ds;         //   decimals
    //Options not implemented
} ntp_packet;

void ntp_setLi(ntp_packet* np,uint32_t L);
void ntp_setVn(ntp_packet* np,uint32_t v);
void ntp_setMode(ntp_packet* np,uint32_t m);
void ntp_setStratum(ntp_packet* np,uint32_t s);
void ntp_setPoll(ntp_packet* np,int32_t s);
void ntp_setPrecision(ntp_packet* np,int32_t s);
void ntp_setRootdelay_s(ntp_packet* np,int32_t s);
void ntp_setRootDispersion_s(ntp_packet* np,int32_t s);
void ntp_setRefClkId(ntp_packet* np,uint32_t s);
void ntp_setRef_s(ntp_packet* np,uint32_t s);
void ntp_setRef_ds(ntp_packet* np,uint32_t s);
void ntp_setOri_s(ntp_packet* np,uint32_t s);
void ntp_setOri_ds(ntp_packet* np,uint32_t s);
void ntp_setRx_s(ntp_packet* np,uint32_t s);
void ntp_setRx_ds(ntp_packet* np,uint32_t s);
void ntp_setTx_s(ntp_packet* np,uint32_t s);
void ntp_setTx_ds(ntp_packet* np,uint32_t s);


//#ifdef DEBUG_NTP
void ntp_info(ntp_packet* info);
