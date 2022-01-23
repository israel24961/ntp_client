#include "ntp_packet.h"

//Needed only for ntp_info and related functions
#include <string.h>
#include <stdio.h>


void ntp_setLi(ntp_packet* np,uint32_t L)
{
    const uint8_t LiMask=0x03;
    const uint8_t LiPos=6;//1100 0000
    int8_t livnmode=L&LiMask;//Cleans Input
    np->Li_Vn_Mode&=~(LiMask<<LiPos);//Cleans LI bits
    np->Li_Vn_Mode|=livnmode<<LiPos;
}
void ntp_setVn(ntp_packet* np,uint32_t v)
{
    const uint8_t VnMask=0x07;
    const uint8_t VnPos=3;//0011 1000
    int8_t livnmode=v&VnMask;//Cleans Input
    np->Li_Vn_Mode&=~(VnMask<<VnPos);//Cleans LI bits
    np->Li_Vn_Mode|=livnmode<<VnPos;
}
void ntp_setMode(ntp_packet* np,uint32_t m)
{
    const uint8_t MoMask=0x07;//0000 0111
    int8_t livnmode=m&MoMask;//Cleans Input
    np->Li_Vn_Mode&=~MoMask;//Cleans LI bits
    np->Li_Vn_Mode|=livnmode;
}
/**Next functions were made with nvim macros, so yeah...
 * I did them because they took less time than writing this
 * comment
 **/
void ntp_setStratum(ntp_packet* np,uint32_t s)
{
    np->Stratum=s;
}
void ntp_setPoll(ntp_packet* np,int32_t s)
{
    np->Poll=s;
}
void ntp_setPrecision(ntp_packet* np,int32_t s)
{
    np->Precision=s;
}
void ntp_setRootdelay_s(ntp_packet* np,int32_t s)
{
    np->Rootdelay_s=s;
}
void ntp_setRootDispersion_s(ntp_packet* np,int32_t s)
{
    np->RootDispersion_s=s;
}
void ntp_setRefClkId(ntp_packet* np,uint32_t s)
{
    np->RefClkId=s;
}
void ntp_setRef_s(ntp_packet* np,uint32_t s)
{
    np->Ref_s=s;
}
void ntp_setRef_ds(ntp_packet* np,uint32_t s)
{
    np->Ref_ds=s;
}
void ntp_setOri_s(ntp_packet* np,uint32_t s)
{
    np->Ori_s=s;
}
void ntp_setOri_ds(ntp_packet* np,uint32_t s)
{
    np->Ori_ds=s;
}
void ntp_setRx_s(ntp_packet* np,uint32_t s)
{
    np->Rx_s=s;
}
void ntp_setRx_ds(ntp_packet* np,uint32_t s)
{
    np->Rx_ds=s;
}
void ntp_setTx_s(ntp_packet* np,uint32_t s)
{
    np->Tx_s=s;
}
void ntp_setTx_ds(ntp_packet* np,uint32_t s)
{
    np->Tx_ds=s;
}

/**
 * Messy functions I had when I was prototyping
 * I won't refactor them....
**/

void info_LeapIndicator(uint8_t Li_Vn_Mode,char* str,uint32_t strlen);
void set_LeapIndicator(uint8_t* Li_Vn_Mode,uint8_t Li);
void info_Mode(uint8_t Li_Vn_Mode,char* str,uint32_t strlen);
uint8_t show_VersionNumber(uint8_t Li_Vn_Mode);

void ntp_info(ntp_packet* info)
{
    char LI_str[50];
    info_LeapIndicator(info->Li_Vn_Mode,LI_str,50);
    char Mode_str[50];
    info_Mode(info->Li_Vn_Mode,Mode_str,50);

    fprintf(stderr, "NTP packet:\n"
            " ---Byte order not respected\n"
            " Li=%s\n"
            " Version=%u\n"
            " Mode=%s\n"
            " Stratum=%u\n"
            " Poll=%u\n"
            " Precision=%u\n"
            " Root Delay[s]=%u\n"
            " Root Dispersion[s]=%u\n"
            " Ref Clock Identifier=%u\n"
            " Ref Tm[s]=%u\n"
            " Ref Tm[ds]=%u\n"
            " Orig Tm[s]=%u\n"
            " Orig Tm[ds]=%u\n"
            " Rx  Tm[s]=%u\n"
            " Rx  Tm[ds]=%u\n"
            ,
            LI_str,show_VersionNumber(info->Li_Vn_Mode),Mode_str,
            info->Stratum,
            info->Poll,
            info->Precision,
            info->Rootdelay_s,
            info->RootDispersion_s,
            info->RefClkId,
            info->Ref_s,
            info->Ref_ds,
            info->Ori_s,
            info->Ori_ds,
            info->Rx_s,
            info->Rx_ds);
}
uint8_t show_VersionNumber(uint8_t Li_Vn_Mode)
{
    const uint8_t VnMask=0x07;
    return Li_Vn_Mode>>3 & VnMask;
}
uint8_t show_Mode(uint8_t Li_Vn_Mode)
{
    const uint8_t LiMask=0x07;
    return Li_Vn_Mode & LiMask;
}
uint8_t show_LeapIndicator(uint8_t Li_Vn_Mode)
{
    const uint8_t LiMask=0x03;
    return Li_Vn_Mode>>6 & LiMask;
}
void set_LeapIndicator(uint8_t* Li_Vn_Mode,uint8_t Li)
{
    const uint8_t LiMask=0x03;
    Li&=LiMask; //JustInCase
    *Li_Vn_Mode&=~(LiMask<<6);
    *Li_Vn_Mode|=Li<<6;
}

//Next functions explain some parameters
void info_Mode(uint8_t Li_Vn_Mode,char* str,uint32_t strlen)
{
    char* messages[]={
        "reserved",
        "symmetric active",
        "symmetric passive",
        "client",
        "server",
        "broadcast",
        "reserved for NTP control message(see Appendix B, RFC 1305)",
        "reserved for NTP control message(see Appendix B, RFC 1305)",
    };
    uint8_t m= show_Mode(Li_Vn_Mode);
    snprintf(str,strlen,"%u::%s",m,messages[m]);
}

void info_LeapIndicator(uint8_t Li_Vn_Mode,char* str,uint32_t strlen)
{
    uint8_t Li=show_LeapIndicator(Li_Vn_Mode);
    switch (Li)
    {
        case 0:
            snprintf(str,strlen,"0b00");
            break;
        case 1:
            snprintf(str,strlen,"0b01 Last minute has 61 seconds");
            break;
        case 2:
            snprintf(str,strlen,"0b10 Last minute has 59 seconds");
            break;
        case 3:
            snprintf(str,strlen,"0b11 alarm condition(clock not synchronized)");
            break;
        default:
            snprintf(str,strlen,"What the f**k! check your code, dude");
            break;
    }
}
