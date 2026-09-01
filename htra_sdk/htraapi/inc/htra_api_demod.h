/**
 * \file htra_api_demod.h
 *
 * \~chinese \brief 解调API（选件）
 *
 * \~chinese 对HTRA频谱分析仪采集的IQ数据进行模拟和数字解调，输出解调结果、星座图、EVM等参数。
 *
 * \~english \brief 
 *
 * \~english 
 */


#ifndef HTRA_API_DEMOD_H
#define HTRA_API_DEMOD_H

#include <stdint.h>
#include "htra_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined _WIN32 || defined __CYGWIN__
#ifdef HTRA_API_EXPORTS
#define HTRA_API __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#else
#define HTRA_API
#endif
#else
#ifdef HTRA_API_EXPORTS
#if __GNUC__ >= 4
#define HTRA_API __attribute__ ((visibility ("default")))
#else
#define HTRA_API
#endif
#else
#define HTRA_API
#endif
#endif


/** \~chinese 解调许可证文件缺失，请将xx._demodlic.txt文件放到/CalFile/文件夹中 */
/** \~english */
#define APIRETVAL_ERROR_DemodLicFileIsMissing -60

/** \~chinese 解调许可证文件内容错误，通常为许可证内容被改动，请联技术支持人员 */
/** \~english */
#define APIRETVAL_ERROR_DemodLicFileIsIncorrect -61

/** \~chinese 请将DigitalSigDemod.dll(Windwos)或libDigitalSigDemod.so(Linux)放到htra_api库的同路径下 */
/** \~english */
#define APIRETVAL_ERROR_NoDemodLib -62

/** \~chinese 打开解调功能失败，通常不会出现此错误，若出现请联技术支持人员 */
/** \~english */
#define APIRETVAL_ERROR_DemodOpenFailed -63

/** \~chinese 输入的采样点数小于16384，请将采样点改为16384及以上 */
/** \~english */
#define APIRETVAL_ERROR_DemodSamplePoints_LessThan_16384 -64

/** \~chinese 输入的 采样点数 大于于320000，请将 采样点数 改为320000及以下 */
/** \~english */
#define APIRETVAL_ERROR_DemodSamplePoints_MoreThan_320000 -65

/** \~chinese 输入的 采样率/符号率 小于4，请将 采样率/符号率 改为4及以上 */
/** \~english */
#define APIRETVAL_ERROR_DemodSampleRateSymbolRateRatio_LessThan_4 -66

/** \~chinese 输入的 采样率/符号率 大于64，请将 采样率/符号率 改为64及以下 */
/** \~english */
#define APIRETVAL_ERROR_DemodSampleRateSymbolRateRatio_LessThan_64 -67

/** \~chinese 输入的 符号率 小于最小值 */
/** \~english */
#define APIRETVAL_ERROR_DemodSymbolRate_LessThan_Min -68

/** \~chinese 输入的 符号率 大于最大值 */
/** \~english */
#define APIRETVAL_ERROR_DemodSymbolRate_MoreThan_Max -69

/** \~chinese 无解调数据输出，当输入的IQ数据存在大量的0值时，则可能出现无法解调的情况 */
/** \~english */
#define APIRETVAL_ERROR_DemodNoOutputData -70

/** \~chinese 解调失败 */
/** \~english */
#define APIRETVAL_ERROR_DemodExecuteFailed -71

/** \~chinese 采样点数 少于推荐值，将返回推荐值 */
/** \~english */
#define APIRETVAL_ERROR_DemodSamplePoints_LessThan_RecommendedValue -72

/** \~chinese 滤波器系数超出可设范围[0.01,0.99]，超出下限则自动返回0.1，超出上限则自动返回0.99 */
/** \~english */
#define APIRETVAL_ERROR_FilterAlpha_OutOfRange -73

/** \~chinese 当前采样率无法设置，已自动设置可设采样率 */
/** \~english */
#define APIRETVAL_ERROR_DemodSampleRate_SetError -74

/** \~chinese 解调许可证过期 */
/** \~english */
#define APIRETVAL_ERROR_DemodLicExpired -75

/**
 * \enum Demod_ModType_TypeDef
 * \~chinese \brief 调制类型
 * \~english \brief 
 */
typedef enum
{
	/** \~chinese 2FSK */
	/** \~english */
	FSK2 = 1,

	/** \~chinese 4FSK */
	/** \~english */
	FSK4,

	/** \~chinese GMSK */
	/** \~english */
	GMSK,

	/** \~chinese BPSK */
	/** \~english */
	BPSK,

	/** \~chinese QPSK */
	/** \~english */
	QPSK,

	/** \~chinese 8PSK */
	/** \~english */
	PSK8,

	/** \~chinese 16QAM */
	/** \~english */
	QAM16,

	/** \~chinese 2ASK */
	/** \~english */
	ASK2,

	/** \~chinese 64QAM */
	/** \~english */
	QAM64,

	/** \~chinese AM */
	/** \~english */
	AM,

	/** \~chinese FM */
	/** \~english */
	FM,

	/** \~chinese PM */
	/** \~english */
	PM,

	/** \~chinese CW */
	/** \~english */
	CW,

	/** \~chinese 低边带 */
	/** \~english */
	LowerSideband,

	/** \~chinese 高边带 */
	/** \~english */
	UpperSideband,

	/** \~chinese 128QAM */
	/** \~english */
	QAM128,

	/** \~chinese 256QAM */
	/** \~english */
	QAM256,

	/** \~chinese 32QAM */
	/** \~english */
	QAM32

}Demod_ModType_TypeDef;

/**
 * \enum Demod_FilterType_TypeDef
 * \~chinese \brief 滤波器类型
 * \~english \brief
 */
typedef enum
{
	/** \~chinese 根升余弦滤波器 */
	/** \~english */
	RootRaisedCosine = 1,

	/** \~chinese 升余弦滤波器 */
	/** \~english */
	RaisedCosine,

	/** \~chinese 高斯滤波器 */
	/** \~english */
	Gaussian,

	/** \~chinese 矩形滤波器 */
	/** \~english */
	Rectangular,

	/** \~chinese 半正弦滤波器 */
	/** \~english */
	HalfSine

}Demod_FilterType_TypeDef;

/**
 * \struct Demod_Profile_TypeDef
 * \~chinese \brief 解调配置结构体
 * \~english \brief
 */
typedef struct
{
	/** \~chinese 采样点数 */
	/** \~english */
	uint64_t SamplePoints;

	/** \~chinese 采样率，Hz */
	/** \~english */
	double SampleRate;

	/** \~chinese 符号率，sym/s */
	/** \~english */
	double SymbolRate;

	/** \~chinese 调制类型 */
	/** \~english */
	Demod_ModType_TypeDef ModType;

	/** \~chinese 仅16APSK可用(暂未开放，设置0即可) */
	/** \~english */
	double APSK16_Gamma;

	/** \~chinese 滤波器类型，目前仅支持RootRaisedCosine */
	/** \~english */
	Demod_FilterType_TypeDef FilterType;

	/** \~chinese 滤波器滚降系数(目前仅支持根升余弦，0.01 <= Alpha <= 0.99) */
	/** \~english */
	double FilterAlpha;

}Demod_Profile_TypeDef;

/**
 * \struct DemodInfo_TypeDef
 * \~chinese \brief 解调信息结构体，包括：眼图、星座图、EVM等，注意：结构体中所有指针变量将指向函数内部的空间，无需用户在外部手动开辟
 * \~english \brief
 */
typedef struct
{
	/** \~chinese 眼图数据起始内存地址 */
	/** \~english */
	double* eDiagram;
	
	/** \~chinese 眼图数据长度 */
	/** \~english */
	uint32_t eDiagram_Len;

	/** \~chinese I路星座图数据起始内存地址 */
	/** \~english */
	double* I_constellation;

	/** \~chinese Q路星座图数据起始内存地址 */
	/** \~english */
	double* Q_constellation;

	/** \~chinese 星座图数据长度 */
	/** \~english */
	uint32_t  constellation_Len;

	/** \~chinese 比特流数据起始内存地址 */
	/** \~english */
	int32_t* bitStream;

	/** \~chinese 比特流数据长度 */
	/** \~english */
	uint32_t bitStream_Len;

	/** \~chinese 码流数据起始内存地址 */
	/** \~english */
	int32_t* symbol;

	/** \~chinese 码流数据长度 */
	/** \~english */
	uint32_t symbol_Len;

	/** \~chinese EVM数据起始内存地址，同为FSK Error、ASK Error % */
	/** \~english */
	double* EVM;

	/** \~chinese EVM数据长度 */
	/** \~english */
	uint32_t EVM_Len;

	/** \~chinese 均方根EVM % */
	/** \~english */
	double EVM_RMS;

	/** \~chinese 峰值EVM % */
	/** \~english */
	double EVM_MAX;

	/** \~chinese 相位误差数据起始内存地址，单位角度 */
	/** \~english */
	double* PhaseError;

	/** \~chinese 相位误差数据长度 */
	/** \~english */
	uint32_t PhaseError_Len;

	/** \~chinese 均方根相位误差，单位角度 */
	/** \~english */
	double PhaseError_RMS;

	/** \~chinese 峰值相位误差，单位角度 */
	/** \~english */
	double PhaseError_MAX;

	/** \~chinese 幅度误差数据起始内存地址 % */
	/** \~english */
	double* MagError;

	/** \~chinese 幅度误差数据长度 */
	/** \~english */
	uint32_t MagError_Len;

	/** \~chinese 均方根幅度误差 % */
	/** \~english */
	double MagError_RMS;

	/** \~chinese 峰值幅度误差 % */
	/** \~english */
	double MagError_MAX;

	/** \~chinese 频率误差，载波相对于中心频率的频率误差，同为CarrFreqOffset，单位Hz */
	/** \~english */
	double FreqError;

	/** \~chinese IQ偏移，单位dB，仅PSK、QAM */
	/** \~english */
	double IQ_Offset;

	/** \~chinese 信噪比，单位dB，仅PSK、QAM */
	/** \~english */
	double SNR;

	/** \~chinese IQ增益不平衡，单位dB，仅PSK、QAM */
	/** \~english */
	double GainImb;

	/** \~chinese IQ正交倾斜误差，单位角度，仅PSK、QAM */
	/** \~english */
	double QuadError;

	/** \~chinese FSK频偏，单位Hz */
	/** \~english */
	double FSK_Deviation;

	/** \~chinese 载波功率，单位dBm，仅ASK */
	/** \~english */
	double CarrPower;

	/** \~chinese ASK调制深度 % */
	/** \~english */
	double ASK_Depth;

	/** \~chinese AM调制深度 % */
	/** \~english */
	double AM_Depth;

	/** \~chinese FM调制频偏，单位Hz */
	/** \~english */
	double FM_Deviation;

	/** \~chinese PM解调数据起始内存地址 */
	/** \~english */
	double* Phase;

	/** \~chinese PM解调数据长度 */
	/** \~english */
	uint32_t Phase_Len;

	/** \~chinese FM解调数据起始内存地址 */
	/** \~english */
	double* Freq;

	/** \~chinese FM解调数据长度 */
	/** \~english */
	uint32_t Freq_Len;

	/** \~chinese AM解调数据起始内存地址 */
	/** \~english */
	double* Amp;

	/** \~chinese AM解调数据长度 */
	/** \~english */
	uint32_t Amp_Len;

	/** \~chinese SSB解调数据起始内存地址，上边带和下边带均用此参数 */
	/** \~english */
	double* SSB;

	/** \~chinese SSB解调数据长度 */
	/** \~english */
	uint32_t SSB_Len;

}DemodInfo_TypeDef;

/**
 * \struct Demod_SymbolMap_TypeDef
 * \~chinese \brief 符号映射表结构体，I为x轴坐标，Q为y轴坐标
 * \~english \brief
 */
typedef struct
{
	/** \~chinese x轴坐标 */
	/** \~english */
	float I;

	/** \~chinese y轴坐标 */
	/** \~english */
	float Q;
}Demod_SymbolMap_TypeDef;

/**
 * \~chinese 核验解调库是否存在
 * \~chinese @return 函数调用状态，0存在，-1不存在
 * \~english
 * \~english @param[in]
 */
 HTRA_API int Demod_Check();

/**
 * \~chinese 打开解调功能，检测是否存在许可证和开辟需要的内存
 * \~chinese @param[in] Device 设备指针
 * \~chinese @return 函数调用状态，0为正常，非0请参考相关宏定义
 *
 * \~english 
 * \~english @param[in] 
 * \~english @return 
 */
HTRA_API int Demod_Open(void** Device);

/**
 * \~chinese 关闭解调功能
 * \~chinese @param[in] Device 设备指针
 * \~chinese @return 函数调用状态，0为正常，非0请参考相关宏定义
 *
 * \~english
 * \~english @param[in]
 * \~english @return
 */
HTRA_API int Demod_Close(void** Device);

/**
 * \~chinese 重置解调功能，当IQ数据不连续时，在每次调用Demod_Execute前，都需要先调用Demod_Reset，若IQ数据连续，则一直调用Demod_Execute即可
 * \~chinese @param[in] Device 设备指针
 * \~chinese @return 函数调用状态，0为正常，非0请参考相关宏定义
 *
 * \~english
 * \~english @param[in]
 * \~english @return
 */
HTRA_API int Demod_Reset(void** Device);

/**
 * \~chinese 获取解调API版本
 * \~chinese @param[in] Device 设备指针
 * \~chinese @param[out] 解调API版本
 * \~chinese @return 返回version的字符长度
 *
 * \~english
 * \~english @param[in]
 * \~english @return
 */
HTRA_API int Demod_GetVersion(void** Device, char version[]);

/**
 * \~chinese 初始化解调配置结构体，将结构体中的每个参数都赋初值
 * \~chinese @param[out] DemodProfile 解调配置结构体
 *
 * \~english
 * \~english @param[in]
 */
HTRA_API void Demod_DeInit(Demod_Profile_TypeDef* DemodProfile);

/**
 * \~chinese 配置解调参数
 * \~chinese @param[in] Device 设备指针
 * \~chinese @param[in] DemodProfileIn 输入解调配置结构体
 * \~chinese @param[out] DemodProfileOut 输出解调配置结构体，若输入配置不合理，将回写输出配置结构体为合理参数
 * \~chinese @return 函数调用状态，0为正常，非0请参考相关宏定义
 *
 * \~english
 * \~english @param[in]
 */
HTRA_API int Demod_Configuration(void** Device, const Demod_Profile_TypeDef* DemodProfileIn, Demod_Profile_TypeDef* DemodProfileOut);

/**
 * \~chinese 执行解调功能
 * \~chinese @param[in] Device 设备指针
 * \~chinese @param[in] IQStream 输入IQ数据结构体
 * \~chinese @param[out] DemodInfo 输出解调信息结构体
 * \~chinese @return 函数调用状态，0为正常，非0请参考相关宏定义
 *
 * \~english
 * \~english @param[in]
 */
HTRA_API int Demod_Execute(void** Device, const IQStream_TypeDef* IQStream, DemodInfo_TypeDef* DemodInfo);

/**
 * \~chinese 生成符号映射表
 * \~chinese @param[in] ModType 调制类型
 * \~chinese @param[out] SymbolMap 符号映射表
 * \~chinese @param[out] MapNum 符号映射表中可用的符号个数
 *
 * \~english
 * \~english @param[in]
 */
HTRA_API void Demod_GenSymbolMap(Demod_ModType_TypeDef ModType, Demod_SymbolMap_TypeDef SymbolMap[1024], uint32_t* MapNum);

#ifdef __cplusplus
}
#endif

#endif