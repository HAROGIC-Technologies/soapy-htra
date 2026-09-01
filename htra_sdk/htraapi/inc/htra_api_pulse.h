/**
 * \file htra_api_pulse.h
 * 
 * \~chinese \brief 脉冲检测API（选件）
 *
 * \~chinese 按照设定阈值，对HTRA频谱分析仪采集的时域数据（dBm或V）进行脉冲检测，输出脉宽、周期、占空比等参数。
 *
 * \~english \brief 
 *
 * \~english 
 */


#ifndef HTRA_API_PULSE_H
#define HTRA_API_PULSE_H

#include <stdint.h>

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


/** \~chinese 脉冲检测许可证文件缺失，请将name_pulsedet.lic（例如：057_20303232424850110024002e_pulsedet.lic）放到/CalFile/文件夹中 */
/** \~english */
#define APIRETVAL_ERROR_PulseDetLicFileIsMissing -50

/** \~chinese 脉冲检测许可证文件内容错误，通常为许可证内容被改动，请联技术支持人员 */
/** \~english */
#define APIRETVAL_ERROR_PulseDetLicFileIsIncorrect -51

/** \~chinese 脉冲检测许可证文件过期，请联技术支持人员 */
/** \~english */
#define APIRETVAL_ERROR_PulseDetLicFileIsExpired -52

/** \~chinese 期望获取的脉冲数量为0，不执行脉冲检测 */
/** \~english */
#define APIRETVAL_ERROR_ExpPulseNumIsZero -53

/** \~chinese 期望获取的脉冲数量大于上限，按照上限执行 */
/** \~english */
#define APIRETVAL_WARNING_ExpPulseNum_MoreThan_Max 50

/** \~chinese 脉冲检测数据点数小于下限，不执行脉冲检测 */
/** \~english */
#define APIRETVAL_ERROR_PulseSize_LessThan_Min -54

/** \~chinese 脉冲检测数据点数大于上限，按照上限执行 */
/** \~english */
#define APIRETVAL_WARNING_PulseSize_MoreThan_Max 51

/** \~chinese 期望获取的脉冲数量最小值 */
/** \~english */
#define APIRETVAL_ExpPulseNumMin 1

/** \~chinese 期望获取的脉冲数量最大值 */
/** \~english */
#define APIRETVAL_ExpPulseNumMax 500000

/** \~chinese 脉冲检测数据点数下限 */
/** \~english */
#define APIRETVAL_ERROR_PulseMinSize 10

/** \~chinese 脉冲检测数据点数上限 */
/** \~english */
#define APIRETVAL_ERROR_PulseMaxSize 500000000

/**
 * \enum Unit_TypeDef
 * \~chinese \brief 数据的单位类型，dBm或V
 * \~english \brief
 */
typedef enum
{
	/** \~chinese 电压 V */
	/** \~english */
	Voltage_V,

	/** \~chinese 功率 dBm */
	/** \~english */
	Power_dBm

}Unit_TypeDef;

/**
 * \struct Pulse_Profile_TypeDef
 * \~chinese \brief 脉冲检测输入数据信息结构体，包括：需要检测的数据、阈值等
 * \~english \brief
 */
typedef struct
{
	/** \~chinese 期望获取的脉冲数量 */
	/** \~english */
	uint32_t ExpPulseNum;

	/** \~chinese 脉冲数据单位，dBm或V */
	/** \~english */
	Unit_TypeDef unit;

	/** \~chinese 脉冲数据的起始内存地址，数据单位取决于unit，当使用Pulse_Detect函数时，需指向DET数据，当使用Pulse_Detect_PM1函数时，需指向IQS数据 */
	/** \~english */
	float* Pulse;

	/** \~chinese 脉冲数据长度 */
	/**\~english */
	uint64_t PulseSize;

	/** \~chinese 脉冲数据时间分辨率，单位秒s */
	/** \~english */
	double TimeResolution_s;

	/** \~chinese 脉冲检测门限，单位与数据保持一致 */
	/** \~english */
	double DetThreshold;

}Pulse_Profile_TypeDef;

/**
 * \struct PulseTDParam_TypeDef
 * \~chinese \brief 脉冲检测时域参数结构体，包括：脉宽、周期、占空比等，单位均为秒s
 * \~english \brief
 */
typedef struct
{
	/** \~chinese 上升时间 */
	/** \~english */
	double RiseTime;

	/** \~chinese 上升沿 */
	/** \~english */
	double RiseEdge;

	/** \~chinese 下降时间 */
	/** \~english */
	double FallTime;

	/** \~chinese 下降沿 */
	/** \~english */
	double FallEdge;

	/** \~chinese 脉宽 */
	/** \~english */
	double Width;

	/** \~chinese 周期 */
	/** \~english */
	double Period;

	/** \~chinese 占空比% */
	/** \~english */
	float DutyCycle;

}PulseTDParam_TypeDef;

/**
 * \struct PulseAMPParam_TypeDef
 * \~chinese \brief 脉冲检测幅度参数结构体，包括：峰值电平、基准电平、峰基比等
 * \~english \brief
 */
typedef struct
{
	/** \~chinese 峰值电平dBm */
	/** \~english */
	float TopLevel_dBm;

	/** \~chinese 峰值电平V */
	/** \~english */
	float TopLevel_V;

	/** \~chinese 基准电平dBm */
	/** \~english */
	float BaseLevel_dBm;

	/** \~chinese 基准电平V */
	/** \~english */
	float BaseLevel_V;

	/** \~chinese 峰基比dB */
	/** \~english */
	float TopToBaseRatio_dB;

	/** \~chinese 峰基差V */
	/** \~english */
	float TopToBaseDiff_V;

	/** \~chinese 下垂dB */
	/** \~english */
	float Droop_dB;

	/** \~chinese 下垂V */
	/** \~english */
	float Droop_V;

	/** \~chinese 过冲dB */
	/** \~english */
	float Overshoot_dB;

	/** \~chinese 过冲V */
	/** \~english */
	float Overshoot_V;

	/** \~chinese 波纹dB */
	/** \~english */
	float Ripple_dB;

	/** \~chinese 波纹V */
	/** \~english */
	float Ripple_V;

}PulseAMPParam_TypeDef;

/**
 * \struct PulseEstParam_TypeDef
 * \~chinese \brief 脉冲检测估计参数结构体
 * \~english \brief
 */
typedef struct
{
	/** \~chinese 10% 电平值所在的数组下标，0为上升沿的下标，1为下降沿的下标 */
	/** \~english */
	double Level_10pct_Index[2];

	/** \~chinese 50% 电平值所在的数组下标，0为上升沿的下标，1为下降沿的下标 */
	/** \~english */
	double Level_50pct_Index[2];

	/** \~chinese 90% 电平值所在的数组下标，0为上升沿的下标，1为下降沿的下标 */
	/** \~english */
	double Level_90pct_Index[2];

	/** \~chinese 95% 电平值所在的数组下标，0为上升沿的下标，1为下降沿的下标 */
	/** \~english */
	double Level_95pct_Index[2];

	/** \~chinese 25% 脉宽位置的数组下标 */
	/** \~english */
	double Width_25pct_Index;

	/** \~chinese 75% 脉宽位置的数组下标 */
	/** \~english */
	double Width_75pct_Index;

	/** \~chinese 推测出的信号与噪声的数据起始下标 */
	/** \~english */
	uint64_t Start_Index;

	/** \~chinese 推测出的信号与噪声的数据长度 */
	/** \~english */
	uint64_t Size;

	/** \~chinese 推测出的噪声数据起始内存地址dBm */
	/** \~english */
	float* Noise_dBm;

	/** \~chinese 推测出的噪声数据起始内存地址V */
	/** \~english */
	float* Noise_V;

	/** \~chinese 推测出的信号数据起始内存地址dBm */
	/** \~english */
	float* Signal_dBm;

	/** \~chinese 推测出的信号数据起始内存地址V */
	/** \~english */
	float* Signal_V;

}PulseEstParam_TypeDef;

/**
 * \struct PulseStatsParam_TypeDef
 * \~chinese \brief 脉冲检测统计参数结构体，包括：最小、最大、平均周期等，单位均为秒s
 * \~english \brief
 */
typedef struct
{
	/** \~chinese 最小周期 */
	/** \~english */
	double MinPRI;

	/** \~chinese 最大周期 */
	/** \~english */
	double MaxPRI;

	/** \~chinese 平均周期 */
	/** \~english */
	double MeanPRI;

	/** \~chinese 最小脉宽 */
	/** \~english */
	double MinPW;

	/** \~chinese 最大脉宽 */
	/** \~english */
	double MaxPW;

	/** \~chinese 平均脉宽 */
	/** \~english */
	double MeanPW;

	/** \~chinese 周期偏差百分比% */
	/** \~english */
	float PRIDeviationPercent;

	/** \~chinese 脉宽偏差百分比% */
	/** \~english */
	float PWDeviationPercent;

}PulseStatsParam_TypeDef;

/**
 * \struct PulseInfo_TypeDef
 * \~chinese \brief 脉冲检测结果结构体，脉冲检测所有参数的汇总
 * \~english \brief
 */
typedef struct
{
	/** \~chinese 实际获取的脉冲数量，按照此数量可从指针变量中获取每个脉冲的的检测结果 */
	/** \~english */
	uint32_t ActPulseNum;

	/** \~chinese 脉冲检测时域参数的起始内存地址 */
	/** \~english */
	PulseTDParam_TypeDef* PulseTDParam;

	/** \~chinese 脉冲检测幅度参数的起始内存地址 */
	/** \~english */
	PulseAMPParam_TypeDef* PulseAMPParam;

	/** \~chinese 脉冲检测绘图参数的起始内存地址 */
	/** \~english */
	PulseEstParam_TypeDef* PulseEstParam;

	/** \~chinese 脉冲检测统计参数 */
	/** \~english */
	PulseStatsParam_TypeDef PulseStats;

}PulseInfo_TypeDef;

/**
 * \struct PulseFreqPhaseParam_TypeDef
 * \~chinese \brief 脉冲的频率和相位信息
 * \~english \brief
 */
typedef struct
{
	/** \~chinese 频率均值 */
	/** \~english */
	double FreqMean;

	/** \~chinese 频率误差 */
	/** \~english */
	double FreqErrorRMS;

	/** \~chinese 相位均值 */
	/** \~english */
	double PhaseMean;

	/** \~chinese 相位误差 */
	/** \~english */
	double PhaseErrorRMS;
}PulseFreqPhaseParam_TypeDef;

/**
 * \struct PulseInfoPM1_TypeDef
 * \~chinese \brief 脉冲检测结果结构体，脉冲检测所有参数的汇总
 * \~english \brief
 */
typedef struct
{
	/** \~chinese 实际获取的脉冲数量，按照此数量可从指针变量中获取每个脉冲的的检测结果 */
	/** \~english */
	uint32_t ActPulseNum;

	/** \~chinese 脉冲检测时域参数的起始内存地址 */
	/** \~english */
	PulseTDParam_TypeDef* PulseTDParam;

	/** \~chinese 脉冲检测幅度参数的起始内存地址 */
	/** \~english */
	PulseAMPParam_TypeDef* PulseAMPParam;

	/** \~chinese 脉冲检测绘图参数的起始内存地址 */
	/** \~english */
	PulseEstParam_TypeDef* PulseEstParam;

	/** \~chinese 脉冲检测统计参数 */
	/** \~english */
	PulseStatsParam_TypeDef PulseStats;

	/** \~chinese 脉冲调制类型，0表示CW，1表示LFM */
	/** \~english */
	uint8_t* Mod;

	/** \~chinese 脉冲的频率和相位信息 */
	/** \~english */
	PulseFreqPhaseParam_TypeDef* PulseFreqPhase;

}PulseInfoPM1_TypeDef;

/**
 * \~chinese \brief 打开脉冲检测功能，检测是否存在许可证和开辟需要的内存
 * \~chinese @param[in] Device 设备指针
 * \~chinese @return 函数调用状态，0为正常，非0请参考相关宏定义
 * \~english \brief
 * \~english @param[in]
 * \~english @return
 */
HTRA_API int Pulse_Open(void** Device);

/**
 * \~chinese \brief 对DET数据执行脉冲检测功能
 * \~chinese @param[in] Device 设备指针
 * \~chinese @param[in] Pulse_Profile 输入脉冲检测数据，包括：需要检测的数据、阈值等
 * \~chinese @param[out] PulseInfo 输出脉冲检测结果，包括：脉宽、周期、占空比等
 * \~chinese @return 函数调用状态，0为正常，非0请参考相关宏定义
 * \~english \brief
 * \~english @param[in]
 * \~english @return
 */
HTRA_API int Pulse_Detect(void** Device, const Pulse_Profile_TypeDef* Pulse_Profile, PulseInfo_TypeDef* PulseInfo);

/**
 * \~chinese \brief 对IQ数据执行脉冲检测功能
 * \~chinese @param[in] Device 设备指针
 * \~chinese @param[in] Pulse_Profile 输入脉冲检测数据，包括：需要检测的数据、阈值等
 * \~chinese @param[out] PulseInfoPM1 输出脉冲检测结果，包括：脉宽、周期、占空比等
 * \~chinese @return 函数调用状态，0为正常，非0请参考相关宏定义
 * \~english \brief
 * \~english @param[in]
 * \~english @return
 */
HTRA_API int Pulse_Detect_PM1(void** Device, const Pulse_Profile_TypeDef* Pulse_Profile, PulseInfoPM1_TypeDef* PulseInfoPM1);

/**
 * \~chinese \brief 关闭脉冲检测功能，释放内存
 * \~chinese @param[in] Device 设备指针
 * \~chinese @return 函数调用状态，0为正常，非0请参考相关宏定义
 * \~english \brief
 * \~english @param[in]
 * \~english @return
 */
HTRA_API int Pulse_Close(void** Device);

#ifdef __cplusplus
}
#endif

#endif