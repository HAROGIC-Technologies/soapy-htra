/** 
 * \file htra_api_pnm.h
 * 
 * \~english \brief Phase Noise Measurement API
 *
 * \~english Performs phase noise measurement and analysis on signals.
 *
 *
 */

 #ifndef HTRA_API_PNM_H
 #define HTRA_API_PNM_H
 
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
 
 
 /** \~english Maximum number of frequency segments (decade segments) */
 #define PHASENOISE_MAXFREQSEGMENT 7
 
 /**
  * \struct PNM_Profile_TypeDef
  * \~english \brief Phase noise measurement configuration structure
  */
 typedef struct
 {
     /** \~english Center frequency */
     double CenterFreq;
 
     /** \~english Carrier detection threshold */
     float Threshold;
 
     /** \~english RBW ratio (RBW of each segment / starting frequency of each segment) */
     double RBWRatio;
 
     /** \~english Start offset frequency */
     double StartOffsetFreq;
 
     /** \~english Stop offset frequency */
     double StopOffsetFreq;
 
     /** \~english Number of trace smoothing iterations */
     uint32_t TraceAverage;  
 
 }PNM_Profile_TypeDef;
 
 /**
  * \struct PNM_MeasInfo_TypeDef
  * \~english \brief Phase noise measurement information structure
  */
 typedef struct
 {
     /** \~english Number of frequency segments (divided by decades) */
     uint32_t Segments;
 
     /** \~english Number of trace points */
     uint32_t TracePoints;
 
     /** \~english Number of trace refreshes per analysis (number of Get interface calls) */
     uint32_t PartialUpdateCounts;
 
     /** \~english Total number of frames in each segment */
     uint32_t FramesInSegment[PHASENOISE_MAXFREQSEGMENT];
 
     /** \~english Frame detection ratio for each segment */
     uint32_t FrameDetRatioOfSegment[PHASENOISE_MAXFREQSEGMENT];
 
     /** \~english Start frequency of each segment */
     double StartFreqOfSegment[PHASENOISE_MAXFREQSEGMENT];
 
     /** \~english Stop frequency of each segment */
     double StopFreqOfSegment[PHASENOISE_MAXFREQSEGMENT];
 
     /** \~english RBW of each segment */
     double RBWOfSegment[PHASENOISE_MAXFREQSEGMENT];
 
 }PNM_MeasInfo_TypeDef;
 
 /**
  * \~english \brief Provides default configuration parameters for phase noise measurement
  * \~english @param[in] Device Device pointer
  * \~english @param[out] PNM_Profile Phase noise measurement configuration structure, refer to structure definition for elements
  * \~english @return Function call status, 0 for normal, non-zero values refer to related macro definitions
  */
 HTRA_API int PNM_ProfileDeInit(void** Device, PNM_Profile_TypeDef* PNM_Profile);
 
 /**
  * \~english \brief Configures phase noise measurement
  * \~english @param[in] Device Device pointer
  * \~english @param[in] PNM_Profile_in Input phase noise measurement configuration structure, refer to structure definition for elements
  * \~english @param[out] PNM_Profile_out Returned phase noise measurement configuration structure, refer to structure definition for elements
  * \~english @param[out] PNM_MeasInfo Measurement information structure for phase noise measurement, refer to structure definition for elements
  * \~english @return Function call status, 0 for normal, non-zero values refer to related macro definitions
  */
 HTRA_API int PNM_Configuration(void** Device, const PNM_Profile_TypeDef* PNM_Profile_in, PNM_Profile_TypeDef* PNM_Profile_out, PNM_MeasInfo_TypeDef* PNM_MeasInfo);
 
 /**
  * \~english \brief Starts a phase noise measurement
  * \~english @param[in] Device Device pointer
  * \~english @return Function call status, 0 for normal, non-zero values refer to related macro definitions
  */
 HTRA_API int PNM_StartMeasure(void** Device);
 
 /**
  * \~english \brief Forcibly stops the current phase noise measurement
  * \~english @param[in] Device Device pointer
  * \~english @return Function call status, 0 for normal, non-zero values refer to related macro definitions
  */
 HTRA_API int PNM_StopMeasure(void** Device);
 
 /**
  * \~english \brief Retrieves the phase noise measurement result trace
  * \~english @param[in] Device Device pointer
  * \~english @param[out] CarrierFreq Carrier frequency
  * \~english @param[out] CarrierPower Carrier power
  * \~english @param[out] Freq Trace frequency axis (in Hz)
  * \~english @param[out] PhaseNoise Trace power axis (in dBc/Hz)
  * \~english @param[out] FrameUpdateCounts Refresh counter (index 0 corresponds to the farthest segment, N to the nearest segment; elements indicate the number of refreshed frames in each segment)
  * \~english @param[out] MeasAuxInfo Auxiliary measurement information structure, refer to the structure definition for elements
  * \~english @return Function call status, 0 for normal, non-zero values refer to related macro definitions
  */
 HTRA_API int PNM_GetPartialUpdatedFullTrace(void** Device, double* CarrierFreq, float* CarrierPower, double Freq[], float PhaseNoise[], uint32_t FrameUpdateCounts[], MeasAuxInfo_TypeDef *MeasAuxInfo, float *RefLevel);
 
 /**
  * \~english \brief Advanced feature: Perform a panoramic scan to detect signals exceeding the carrier threshold
  * \~english @param[in] Device Device pointer
  * \~english @param[out] CarrierFreq Carrier frequency
  * \~english @param[out] Found Carrier presence flag
  * \~english @return Function call status, 0 for normal, non-zero values refer to related macro definitions
  */
 HTRA_API int PNM_AutoSearch(void** Device, double* CarrierFreq, uint8_t* Found);
 
 /**
  * \~english \brief Advanced feature: Reset the frame detection ratio for each frequency segment
  * \~english @param[in] Device Device pointer
  * \~english @param[out] MeasInfo Updated phase noise measurement information structure after resetting the frame detection ratio
  * \~english @return Function call status, 0 for normal, non-zero values refer to related macro definitions
  */
 HTRA_API int PNM_Preset_FrameDetRatio(void** Device, PNM_MeasInfo_TypeDef* MeasInfo);
 
 /**
  * \~english \brief Advanced feature: Manually configure the frame detection ratio for each frequency segment
  * \~english @param[in] Device Device pointer
  * \~english @param[in] FrameDetRatioOfSegment Array for setting frame detection ratios (array length equals the number of segments; index 0 corresponds to the farthest segment, N to the nearest segment)
  * \~english @param[out] MeasInfo Updated phase noise measurement information structure after adjusting the frame detection ratio
  * \~english @return Function call status, 0 for normal, non-zero values refer to related macro definitions
  */
 HTRA_API int PNM_Set_FrameDetRatio(void **Device, uint32_t FrameDetRatioOfSegment[], PNM_MeasInfo_TypeDef* MeasInfo);
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif
 