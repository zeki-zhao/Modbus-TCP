 /**
 * <interfacename>CmpModbusTCP</interfacename>
 * <description></description>
 *
 * <copyright></copyright>
 */


	
	
#ifndef _CMPMODBUSTCPITF_H_
#define _CMPMODBUSTCPITF_H_

#include "CmpStd.h"

 

 




/** EXTERN LIB SECTION BEGIN **/
/*  Comments are ignored for m4 compiler so restructured text can be used.  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * <description>__ARRAY__DIM__INFO</description>
 */
typedef struct tag__ARRAY__DIM__INFO
{
	RTS_IEC_DINT diLower;		
	RTS_IEC_DINT diUpper;		
} __ARRAY__DIM__INFO;

/**
 * <description>modbus_tcp_client__main</description>
 */
typedef struct tagmodbus_tcp_client_struct
{
	void* __VFTABLEPOINTER;				/* Pointer to virtual function table */

	/* Member variables of Modbus_TCP_Client */

	RTS_IEC_BOOL Enable;				/* VAR_INPUT */	/* 使能开关（1：开启，0：关闭） */
	RTS_IEC_STRING IP[81];				/* VAR_INPUT */	/* 从站ip地址,例如'192.168.0.1' */
	RTS_IEC_WORD Port;					/* VAR_INPUT */	/* 客户端端口号,例如502 */
	RTS_IEC_BYTE Unit;					/* VAR_INPUT */	/* 从站单元号,例如1 */
	RTS_IEC_BYTE RW;					/* VAR_INPUT */	/* 读写标志(0:读 1:写) */
	RTS_IEC_DWORD Addr;					/* VAR_INPUT */	/* 读写寄存器地址(比如 40001,30001) */
	RTS_IEC_INT Count;					/* VAR_INPUT */	/* 读写字节数量(取值范围0-120个字) */
	RTS_IEC_UINT *DataPtr;				/* VAR_INPUT */	/* 读写指针(存放读取到的数据的位置或存放要写到寄存器的数据) */
	RTS_IEC_BOOL Done;					/* VAR_OUTPUT */	/* Input_Reg:ARRAY [0..299] OF UINT;
 完成位(0:指令正在执行 1:指令执行完成) */
	RTS_IEC_BYTE Error;					/* VAR_OUTPUT */	/* 错误码 */
	RTS_IEC_UINT *DataReg;				/* VAR_INPUT */	/* 存放读取到的数据或存放要写到寄存器的数据 */
	__ARRAY__DIM__INFO DataReg__Array__Info[1];	/* VAR_INPUT */	
} modbus_tcp_client_struct;

/**
 * <description>modbus_tcp_client_main</description>
 */
typedef struct tagmodbus_tcp_client_main_struct
{
	modbus_tcp_client_struct *pInstance;	/* VAR_INPUT */	
} modbus_tcp_client_main_struct;

void CDECL CDECL_EXT modbus_tcp_client__main(modbus_tcp_client_main_struct *p);
typedef void (CDECL CDECL_EXT* PFMODBUS_TCP_CLIENT__MAIN_IEC) (modbus_tcp_client_main_struct *p);
#if defined(CMPMODBUSTCP_NOTIMPLEMENTED) || defined(MODBUS_TCP_CLIENT__MAIN_NOTIMPLEMENTED)
	#define USE_modbus_tcp_client__main
	#define EXT_modbus_tcp_client__main
	#define GET_modbus_tcp_client__main(fl)  ERR_NOTIMPLEMENTED
	#define CAL_modbus_tcp_client__main(p0) 
	#define CHK_modbus_tcp_client__main  FALSE
	#define EXP_modbus_tcp_client__main  ERR_OK
#elif defined(STATIC_LINK)
	#define USE_modbus_tcp_client__main
	#define EXT_modbus_tcp_client__main
	#define GET_modbus_tcp_client__main(fl)  CAL_CMGETAPI( "modbus_tcp_client__main" ) 
	#define CAL_modbus_tcp_client__main  modbus_tcp_client__main
	#define CHK_modbus_tcp_client__main  TRUE
	#define EXP_modbus_tcp_client__main  s_pfCMRegisterAPI2( (const CMP_EXT_FUNCTION_REF*)"modbus_tcp_client__main", (RTS_UINTPTR)modbus_tcp_client__main, 1, 0x5A008151, 0x03050900) 
#elif defined(MIXED_LINK) && !defined(CMPMODBUSTCP_EXTERNAL)
	#define USE_modbus_tcp_client__main
	#define EXT_modbus_tcp_client__main
	#define GET_modbus_tcp_client__main(fl)  CAL_CMGETAPI( "modbus_tcp_client__main" ) 
	#define CAL_modbus_tcp_client__main  modbus_tcp_client__main
	#define CHK_modbus_tcp_client__main  TRUE
	#define EXP_modbus_tcp_client__main  s_pfCMRegisterAPI2( (const CMP_EXT_FUNCTION_REF*)"modbus_tcp_client__main", (RTS_UINTPTR)modbus_tcp_client__main, 1, 0x5A008151, 0x03050900) 
#elif defined(CPLUSPLUS_ONLY)
	#define USE_CmpModbusTCPmodbus_tcp_client__main
	#define EXT_CmpModbusTCPmodbus_tcp_client__main
	#define GET_CmpModbusTCPmodbus_tcp_client__main  ERR_OK
	#define CAL_CmpModbusTCPmodbus_tcp_client__main  modbus_tcp_client__main
	#define CHK_CmpModbusTCPmodbus_tcp_client__main  TRUE
	#define EXP_CmpModbusTCPmodbus_tcp_client__main  s_pfCMRegisterAPI2( (const CMP_EXT_FUNCTION_REF*)"modbus_tcp_client__main", (RTS_UINTPTR)modbus_tcp_client__main, 1, 0x5A008151, 0x03050900) 
#elif defined(CPLUSPLUS)
	#define USE_modbus_tcp_client__main
	#define EXT_modbus_tcp_client__main
	#define GET_modbus_tcp_client__main(fl)  CAL_CMGETAPI( "modbus_tcp_client__main" ) 
	#define CAL_modbus_tcp_client__main  modbus_tcp_client__main
	#define CHK_modbus_tcp_client__main  TRUE
	#define EXP_modbus_tcp_client__main  s_pfCMRegisterAPI2( (const CMP_EXT_FUNCTION_REF*)"modbus_tcp_client__main", (RTS_UINTPTR)modbus_tcp_client__main, 1, 0x5A008151, 0x03050900) 
#else /* DYNAMIC_LINK */
	#define USE_modbus_tcp_client__main  PFMODBUS_TCP_CLIENT__MAIN_IEC pfmodbus_tcp_client__main;
	#define EXT_modbus_tcp_client__main  extern PFMODBUS_TCP_CLIENT__MAIN_IEC pfmodbus_tcp_client__main;
	#define GET_modbus_tcp_client__main(fl)  s_pfCMGetAPI2( "modbus_tcp_client__main", (RTS_VOID_FCTPTR *)&pfmodbus_tcp_client__main, (fl) | CM_IMPORT_EXTERNAL_LIB_FUNCTION, 0x5A008151, 0x03050900)
	#define CAL_modbus_tcp_client__main  pfmodbus_tcp_client__main
	#define CHK_modbus_tcp_client__main  (pfmodbus_tcp_client__main != NULL)
	#define EXP_modbus_tcp_client__main   s_pfCMRegisterAPI2( (const CMP_EXT_FUNCTION_REF*)"modbus_tcp_client__main", (RTS_UINTPTR)modbus_tcp_client__main, 1, 0x5A008151, 0x03050900) 
#endif


/**
 * <description>modbus_tcp_server__main</description>
 */
typedef struct tagmodbus_tcp_server_struct
{
	void* __VFTABLEPOINTER;				/* Pointer to virtual function table */

	/* Member variables of Modbus_TCP_Server */

	RTS_IEC_BOOL Enable;				/* VAR_INPUT */	/* 激活位(0:非激活 1:激活)，没激活一次，指令执行一次。 */
	RTS_IEC_WORD Port;					/* VAR_INPUT */	/* 端口号 */
	RTS_IEC_BYTE Unit;					/* VAR_INPUT */	/* 从站单元号,例如1 */
	RTS_IEC_DINT Delay;					/* VAR_INPUT */	/* 回复延时时间(一般设置为0即可，如其他通讯参数均正确但仍无法通讯，可适当增加该值) */
	RTS_IEC_BYTE *InputsPtr;			/* VAR_INPUT */	/* 输入寄存器指针 */
	RTS_IEC_BYTE *OutputsPtr;			/* VAR_INPUT */	/* 保持寄存器指针 */
	RTS_IEC_BOOL Done;					/* VAR_OUTPUT */	/* 完成位(0:指令正在执行 1:指令执行完成) */
	RTS_IEC_BYTE Error;					/* VAR_OUTPUT */	/* 错误码 */
	RTS_IEC_UINT *Inputs;				/* VAR_INPUT */	/* 输入寄存器数组 */
	RTS_IEC_UINT *Outputs;				/* VAR_INPUT */	/* 保持寄存器数组 */
	__ARRAY__DIM__INFO Inputs__Array__Info[1];	/* VAR_INPUT */	
	__ARRAY__DIM__INFO Outputs__Array__Info[1];	/* VAR_INPUT */	
} modbus_tcp_server_struct;

/**
 * <description>modbus_tcp_server_main</description>
 */
typedef struct tagmodbus_tcp_server_main_struct
{
	modbus_tcp_server_struct *pInstance;	/* VAR_INPUT */	
} modbus_tcp_server_main_struct;

void CDECL CDECL_EXT modbus_tcp_server__main(modbus_tcp_server_main_struct *p);
typedef void (CDECL CDECL_EXT* PFMODBUS_TCP_SERVER__MAIN_IEC) (modbus_tcp_server_main_struct *p);
#if defined(CMPMODBUSTCP_NOTIMPLEMENTED) || defined(MODBUS_TCP_SERVER__MAIN_NOTIMPLEMENTED)
	#define USE_modbus_tcp_server__main
	#define EXT_modbus_tcp_server__main
	#define GET_modbus_tcp_server__main(fl)  ERR_NOTIMPLEMENTED
	#define CAL_modbus_tcp_server__main(p0) 
	#define CHK_modbus_tcp_server__main  FALSE
	#define EXP_modbus_tcp_server__main  ERR_OK
#elif defined(STATIC_LINK)
	#define USE_modbus_tcp_server__main
	#define EXT_modbus_tcp_server__main
	#define GET_modbus_tcp_server__main(fl)  CAL_CMGETAPI( "modbus_tcp_server__main" ) 
	#define CAL_modbus_tcp_server__main  modbus_tcp_server__main
	#define CHK_modbus_tcp_server__main  TRUE
	#define EXP_modbus_tcp_server__main  s_pfCMRegisterAPI2( (const CMP_EXT_FUNCTION_REF*)"modbus_tcp_server__main", (RTS_UINTPTR)modbus_tcp_server__main, 1, 0x26DD5F66, 0x03050900) 
#elif defined(MIXED_LINK) && !defined(CMPMODBUSTCP_EXTERNAL)
	#define USE_modbus_tcp_server__main
	#define EXT_modbus_tcp_server__main
	#define GET_modbus_tcp_server__main(fl)  CAL_CMGETAPI( "modbus_tcp_server__main" ) 
	#define CAL_modbus_tcp_server__main  modbus_tcp_server__main
	#define CHK_modbus_tcp_server__main  TRUE
	#define EXP_modbus_tcp_server__main  s_pfCMRegisterAPI2( (const CMP_EXT_FUNCTION_REF*)"modbus_tcp_server__main", (RTS_UINTPTR)modbus_tcp_server__main, 1, 0x26DD5F66, 0x03050900) 
#elif defined(CPLUSPLUS_ONLY)
	#define USE_CmpModbusTCPmodbus_tcp_server__main
	#define EXT_CmpModbusTCPmodbus_tcp_server__main
	#define GET_CmpModbusTCPmodbus_tcp_server__main  ERR_OK
	#define CAL_CmpModbusTCPmodbus_tcp_server__main  modbus_tcp_server__main
	#define CHK_CmpModbusTCPmodbus_tcp_server__main  TRUE
	#define EXP_CmpModbusTCPmodbus_tcp_server__main  s_pfCMRegisterAPI2( (const CMP_EXT_FUNCTION_REF*)"modbus_tcp_server__main", (RTS_UINTPTR)modbus_tcp_server__main, 1, 0x26DD5F66, 0x03050900) 
#elif defined(CPLUSPLUS)
	#define USE_modbus_tcp_server__main
	#define EXT_modbus_tcp_server__main
	#define GET_modbus_tcp_server__main(fl)  CAL_CMGETAPI( "modbus_tcp_server__main" ) 
	#define CAL_modbus_tcp_server__main  modbus_tcp_server__main
	#define CHK_modbus_tcp_server__main  TRUE
	#define EXP_modbus_tcp_server__main  s_pfCMRegisterAPI2( (const CMP_EXT_FUNCTION_REF*)"modbus_tcp_server__main", (RTS_UINTPTR)modbus_tcp_server__main, 1, 0x26DD5F66, 0x03050900) 
#else /* DYNAMIC_LINK */
	#define USE_modbus_tcp_server__main  PFMODBUS_TCP_SERVER__MAIN_IEC pfmodbus_tcp_server__main;
	#define EXT_modbus_tcp_server__main  extern PFMODBUS_TCP_SERVER__MAIN_IEC pfmodbus_tcp_server__main;
	#define GET_modbus_tcp_server__main(fl)  s_pfCMGetAPI2( "modbus_tcp_server__main", (RTS_VOID_FCTPTR *)&pfmodbus_tcp_server__main, (fl) | CM_IMPORT_EXTERNAL_LIB_FUNCTION, 0x26DD5F66, 0x03050900)
	#define CAL_modbus_tcp_server__main  pfmodbus_tcp_server__main
	#define CHK_modbus_tcp_server__main  (pfmodbus_tcp_server__main != NULL)
	#define EXP_modbus_tcp_server__main   s_pfCMRegisterAPI2( (const CMP_EXT_FUNCTION_REF*)"modbus_tcp_server__main", (RTS_UINTPTR)modbus_tcp_server__main, 1, 0x26DD5F66, 0x03050900) 
#endif


#ifdef __cplusplus
}
#endif

/** EXTERN LIB SECTION END **/




typedef struct
{
	IBase_C *pBase;
} ICmpModbusTCP_C;

#ifdef CPLUSPLUS
class ICmpModbusTCP : public IBase
{
	public:
};
	#ifndef ITF_CmpModbusTCP
		#define ITF_CmpModbusTCP static ICmpModbusTCP *pICmpModbusTCP = NULL;
	#endif
	#define EXTITF_CmpModbusTCP
#else	/*CPLUSPLUS*/
	typedef ICmpModbusTCP_C		ICmpModbusTCP;
	#ifndef ITF_CmpModbusTCP
		#define ITF_CmpModbusTCP
	#endif
	#define EXTITF_CmpModbusTCP
#endif

#ifdef CPLUSPLUS_ONLY
  #undef CPLUSPLUS_ONLY
#endif

#endif /*_CMPMODBUSTCPITF_H_*/
