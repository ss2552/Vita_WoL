#include <vitasdk.h>
// SceIofilemgr_stub SceIofilemgrForDriver_stub SceNet_stub SceSysmodule_stub

#define PORT 9
#define MAGIC_BUFFER 102 // 6+(6*16)
#define NET_PARAM_MEM_SIZE (1*1024*1024)
#define SIZE_IS_A_SIX 6
#define FF_MA 12

int user_msg_dialog(const SceChar8 *msg){
	SceMsgDialogParam DialogParam;
	SceMsgDialogUserMessageParam DialogUserMessageParam;

	sceMsgDialogParamInit(&DialogParam);
	sceClibMemset(&DialogUserMessageParam, 0, sizeof(DialogUserMessageParam));
	
	DialogParam.mode = SCE_MSG_DIALOG_MODE_USER_MSG;
	DialogParam.userMsgParam = &DialogUserMessageParam;
	DialogUserMessageParam.buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_OK;
	DialogUserMessageParam.msg = &msg;

	sceMsgDialogInit(&DialogParam);
	
	int auto_exit_counter = 10;
	while(sceMsgDialogGetStatus() != SCE_COMMON_DIALOG_STATUS_FINISHED){
		sceKernelDelayThread(1000000);
		if(--auto_exit_counter)break;
	}
	
	sceMsgDialogTerm();
	return sceMsgDialogClose();
}

int sys_msg_dialog(const int sysMsgType){
	SceMsgDialogParam DialogParam;
	SceMsgDialogSystemMessageParam DialogSystemMessageParam;

	sceMsgDialogParamInit(&DialogParam);
	sceClibMemset(&DialogSystemMessageParam, 0, sizeof(DialogSystemMessageParam));

	DialogParam.mode = SCE_MSG_DIALOG_MODE_SYSTEM_MSG;
	DialogParam.sysMsgParam = &DialogSystemMessageParam;
	DialogSystemMessageParam.sysMsgType = sysMsgType;
	sceMsgDialogInit(&DialogParam);
	
	int auto_exit_counter = 10;
	while(sceMsgDialogGetStatus() != SCE_COMMON_DIALOG_STATUS_FINISHED){
		sceKernelDelayThread(1000000);
		if(--auto_exit_counter)break;
	};

	sceMsgDialogTerm();
	return sceMsgDialogClose();
}

int error_msg_dialog(const char msg){
	user_msg_dialog(&msg);
	return sceKernelExitProcess(-1);
}

int main(){
	sys_msg_dialog(SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_WIFI_REQUIRED_APPLICATION);
	SceUID fd = sceIoOpen("ux:/Vita_WoL_macaddress", (SCE_O_RDONLY | SCE_O_RDLOCK), SCE_S_IRUSR);
	if(fd < 0)return error_msg_dialog("ファイルが開けん");

	uint32_t *payload_buffer_address = malloc(MAGIC_BUFFER);
	sce_paf_memset(payload_buffer_address, 0xFF, SIZE_IS_A_SIX);
	sceIoRead(fd, payload_buffer_address[SIZE_IS_A_SIX], SIZE_IS_A_SIX);
	sceIoClose(fd);
	if(sizeof payload_buffer_address == FF_MA)return error_msg_dialog("6バイト トータル12バイト じゃ無い");

	for(uint32_t addr = &payload_buffer_address[FF_MA]; sizeof payload_buffer_address < MAGIC_BUFFER; addr+=SIZE_IS_A_SIX)
		memcpy(addr, payload_buffer_address[FF_MA], sizeof payload_buffer_address - SIZE_IS_A_SIX);
		
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	struct SceNetInitParam NetInitParam;
	sce_paf_memset(&NetInitParam, 0, sizeof NetInitParam);
	NetInitParam.memory = malloc(NET_PARAM_MEM_SIZE);
	sce_paf_memset(NetInitParam.memory, 0, NET_PARAM_MEM_SIZE);
	NetInitParam.size = NET_PARAM_MEM_SIZE;
	NetInitParam.flags = 0;
	sceNetInit(&NetInitParam);

	int32_t s = sceNetSocket("Vita_WoL", SCE_NET_AF_INET, SCE_NET_SOCK_DGRAM, SCE_NET_IPPROTO_UDP);
	if(!s)return error_msg_dialog("送信出来んかった");
	
	struct SceNetSockaddrIn serv_addr;
	serv_addr.sin_family = SCE_NET_AF_INET;
	serv_addr.sin_port = sceNetHtons(PORT);
	serv_addr.sin_addr.s_addr = SCE_NET_INADDR_BROADCAST;
	sceNetSendto(s, &payload_buffer_address, MAGIC_BUFFER, 0, (SceNetSockaddr *)&serv_addr, sizeof serv_addr);
	sceNetSocketClose(s);
	
	NetTerm(); sceNetTerm(); sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
	return 0;
}
