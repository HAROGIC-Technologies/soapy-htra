/**
 * \file htra_api_emi.h
 * 
 * \~chinese \brief 用作EMI功能的频段预扫描与频点测量API
 *
 * \~chinese 预扫描时，设备以Sweep模式获取频段的迹线；频点测量时，设备以IQS模式分析频点的功率（电压）。
 *
 * \~english \brief 
 *
 * \~english 
 */


#ifndef HTRA_API_EMI_H
#define HTRA_API_EMI_H

#include <stdint.h>
#include "htra_api.h"

/** \~chinese EMI许可证文件缺失，请将name_emi.lic（例如：057_20303232424850110024002e_emi.lic）放到/CalFile/文件夹中 */
/** \~english */
#define APIRETVAL_ERROR_EMILicFileIsMissing -53

/** \~chinese EMI许可证文件内容错误，通常为许可证内容被改动，请联系技术支持人员 */
/** \~english */
#define APIRETVAL_ERROR_EMILicFileIsIncorrect -54

/** \~chinese EMI许可证文件过期，请联系技术支持人员 */
/** \~english */
#define APIRETVAL_ERROR_EMILicFileIsExpired -55

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

/**
 * @brief 单次search到的SIGNAL个数上限
 */
#define EMI_MAX_SIGNALS 256

/**
 * @brief EMI预扫描的检波器枚举。
 */
enum emi_detector : uint8_t 
{
    EMI_SAMPLE_DETECTOR = 0x01,
    EMI_MAX_DETECTOR = 0x02,
    EMI_RMS_DETECTOR = 0x04,
    EMI_MIN_DETECTOR = 0x08,
    EMI_ALL_DETECTOR = 0x0f
};

/**
 * @brief EMI预扫描的窗函数。
 */
enum emi_window : uint8_t 
{
    CISPR = 0x01,
};

/**
 * @brief 搜索方法
 */
enum search_mode
{
    SPUR, // 与限制线相比寻点
    PEAK  // 与局部噪底相比寻点
};

/**
 * @brief EMI预扫描配置参数。
 */
typedef struct
{
    /** 扫描起始频率，单位 Hz */
    double start;

    /** 扫描终止频率，单位 Hz */
    double stop;

    /** 参考电平，单位由 unit 指定 */
    float ref_level;

    /** RBW带宽，单位 Hz */
    double rbw;

    /** RBW带宽定义点，EMI/CISPR通常为6 dB */
    float filter_dB;

    /** VBW带宽，单位 Hz */
    double vbw;

    /** 单个频点驻留时间，单位 s */
    float dwell_time;

    /** 幅度单位 */
    dB_unit unit;

    /** 扫描检波器，可由 emi_detector 按位组合 */
    uint8_t detector;

    /** FFT窗型 */
    emi_window window;

}emi_scan_profile;

/**
 * @brief EMI预扫描返回信息。
 */
typedef struct
{
    /** 迹线第一个点的频率，单位 Hz */
    double first_freq;

    /** 迹线最后一个点的频率，单位 Hz */
    double last_freq;

    /** 完整迹线总点数 */
	uint32_t total_points;
  
    /** 分段扫描数量 */
	uint32_t sections;

    /** 标准分段点数 */
    uint32_t section_points;    
 
    /** 最后一段点数 */
    uint32_t last_section_points;

}emi_scan_info;

/**
 * @brief EMI信号搜索结果。
 */
typedef struct
{
    /** 超门限区段内最高功率点的频率，单位 Hz */
    double freq;

    /** 超门限区段内最高功率点的幅度，单位与输入迹线一致 */
    float level;

    /** 连续超门限区段的带宽，单位 Hz */
    double bw;

    /** 最高功率点超过检测门限的幅度，单位 dB */
    float over_db;

} spur;

/**
 * @brief EMI频点测量配置参数。
 */
typedef struct
{
    /** 测量中心频率，单位 Hz */
    double center;

    /** 参考电平，单位由 unit 指定 */
    float ref_level;

    /** 测量带宽，单位 Hz */
    double bw;
 
    /** RBW带宽定义点，EMI/CISPR通常为6 dB */
    float filter_dB;

    /** 准峰值检波器充电时间，单位 s */
    float charge_time;

    /** 准峰值检波器放电时间，单位 s */
    float decay_time;

    /** 测量时间，单位 s，最小0.1 s；mode=0时向上对齐为0.1 s整数倍，mode=-1时失效 */
    float measure_time;

    /** 幅度单位 */
    dB_unit unit;

    /** 测量模式：0-测满 根据用户设定时间测量；-1-自动进行2s测量 */
    int8_t mode;

}emi_measure_profile;

/**
 * @brief 配置EMI预扫描频段参数。
 *
 * @param[in]  device          设备句柄
 * @param[in]  scan_profile_i  输入预扫描配置参数
 * @param[out] scan_profile_o  返回实际生效的预扫描配置参数
 * @param[out] scan_info_o     返回预扫描迹线信息
 *
 * @return 0 成功，非 0 失败
 */
HTRA_API int emi_config_scan(void** device, const emi_scan_profile* scan_profile_i, emi_scan_profile* scan_profile_o, emi_scan_info* scan_info_o);

/**
 * @brief 获取EMI预扫描分段迹线。
 *
 * @param[in]  device        设备句柄
 * @param[out] freq_o        返回频率轴，单位 Hz
 * @param[out] amp_peak_o    返回Peak功率轴，单位由 scan_profile_i.unit 指定
 * @param[out] amp_rms_o     返回RMS功率轴，单位由 scan_profile_i.unit 指定
 * @param[out] amp_min_o     返回Min功率轴，单位由 scan_profile_i.unit 指定
 * @param[out] amp_sample_o  返回Sample功率轴，单位由 scan_profile_i.unit 指定
 * @param[out] index_o       返回当前分段索引，用于拼接完整迹线
 * @param[out] aux_info_o    返回辅助测量信息
 * @param[out] completed_o   返回频段扫描完成标志，1表示完成
 *
 * @return 0 成功，非 0 失败
 */
HTRA_API int emi_get_partial_trace(void** device, double freq_o[], float amp_peak_o[], float amp_rms_o[], float amp_min_o[], float amp_sample_o[], int* index_o, MeasAuxInfo_TypeDef* aux_info_o, uint8_t* completed_o);

/**
 * @brief 将预扫描迹线与EMI标准进行比较，寻找异常信号。
 * 如果标准的limit是Peak 或 QP标准，请传入Peak功率轴
 * 如果标准的limit是RMS 或 linAvg标准，请传入RMS功率轴
 *
 * @param[in] mode            搜索模式
 * @param[in] freq            trace频率数组
 * @param[in] amplitude       trace功率数组
 * @param[in] points          trace点数。
 * @param[in] limit_freq      SPUR模式下的限制线频点数组，PEAK模式下忽略
 * @param[in] limit_amplitude SPUR模式下的限制线幅度数组，PEAK模式下忽略（和trace同单位）
 * @param[in] limit_points    限制线点数
 * @param[in] margin          SPUR模式下以limit+margin作为搜索门限，推荐范围-20～20 dB；
 *                            PEAK模式下以噪底+margin作为搜索门限，推荐范围6～42 dB。
 * @param[out] result_o       检测结果数组，容量上限为256
 * @param[out] signalnums_o   实际输出结果数量
 *
 * @return 0 成功，非 0 失败
 */
HTRA_API void emi_search(search_mode mode, double freq[], float amplitude[], uint32_t points, double limit_freq[], float limit_amplitude[], uint32_t limit_points, float margin, spur result_o[], uint32_t* signalnums_o);

/**
 * @brief 配置EMI频点测量参数。
 *
 * @param[in]  device         设备句柄
 * @param[in]  meas_profile_i  输入频点测量配置参数
 * @param[out] meas_profile_o  返回实际生效的频点测量配置参数
 *
 * @return 0 成功，非 0 失败
 */
HTRA_API int emi_config_measure(void** device, const emi_measure_profile* meas_profile_i, emi_measure_profile* meas_profile_o);

/**
 * @brief 获取EMI频点测量结果。
 *
 * @param[in]  device          设备句柄
 * @param[out] peak_o          返回峰值检波结果，单位由 measprofile_i.unit 指定
 * @param[out] rms_avg_o       返回RMS检波结果，单位由 measprofile_i.unit 指定
 * @param[out] lin_avg_o       返回线性平均检波结果，单位由 measprofile_i.unit 指定
 * @param[out] quasi_peak_o    返回准峰值检波结果，单位由 measprofile_i.unit 指定
 * @param[out] measure_time_o  返回已测量时间，单位 s
 * @param[out] aux_info_o      返回辅助测量信息
 * @param[out] completed_o     返回频点测量完成标志，1表示完成
 *
 * @note completed_o为0时，Peak、RMS、Avg和QP检波结果均返回0；completed_o为1时返回本轮测量的最终结果。
 * @note QP充放电状态经过CISPR临界阻尼表头，返回整段有效测量时间内的表头输出最大值；两级状态均跨100 ms内部窗口连续保留。
 * @note mode为-1时有效measure_time固定为2.0 s，不再进行QP delta收敛判断或5 s超时测量。
 *
 * @return 0 成功，非 0 失败
 */
HTRA_API int emi_get_measure_result(void** device, float* peak_o, float* rms_avg_o, float* lin_avg_o, float* quasi_peak_o, float* measure_time_o, MeasAuxInfo_TypeDef* aux_info_o, uint8_t* completed_o);

/**
 * @brief 启动EMI频点测量。
 *
 * @param[in] device  设备句柄
 *
 * @return 0 成功，非 0 失败
 */
HTRA_API int emi_start_measure(void** device);

/**
 * @brief 停止EMI频点测量。
 *
 * @param[in] device  设备句柄
 *
 * @return 0 成功，非 0 失败
 */
HTRA_API int emi_stop_measure(void** device);

/**
 * @brief 复位EMI频点测量。
 *
 * @param[in] device  设备句柄
 *
 * @return 0 成功，非 0 失败
 */
HTRA_API int emi_reset_measure(void** device);

#ifdef __cplusplus
}
#endif

#endif
