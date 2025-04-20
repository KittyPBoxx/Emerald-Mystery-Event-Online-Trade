/****************************************************************************
 * Multiboot Script Loader
 * KittyPBoxx 2024
 *
 * seriallink.h
 * Hide all the nasty communication callback stuff for async transmission
 ***************************************************************************/
#include <ogcsys.h>
#include <gccore.h>
#include <string.h>

void switchToSlowTransfer();
void ch0TypeCallback(s32 res, u32 val);
void ch1TypeCallback(s32 res, u32 val);
void ch2TypeCallback(s32 res, u32 val);
void ch3TypeCallback(s32 res, u32 val);
SICallback SL_getDeviceTypeCallback(u8 channel);
void SL_resetDeviceType(u8 channel);
u32 SL_getDeviceType(u8 channel);
void ch0TransmissionFinishedCallback(s32 res, u32 val);
void ch1TransmissionFinishedCallback(s32 res, u32 val);
void ch2TransmissionFinishedCallback(s32 res, u32 val);
void ch3TransmissionFinishedCallback(s32 res, u32 val);
SICallback SL_getTransmissionFinishedCallback(u8 channel);
void SL_resetTransmissionFinished(u8 channel);
u32 SL_isTransmissionFinished(u8 channel);
int SL_recv(u8 channel, u8 pktIn[4]);
int SL_send(u8 channel, u32 msg);
int SL_reset(u8 channel, u8 pktIn[4]);
int SL_getstatus(u8 channel, u8 pktIn[4]);

