#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "qf_sys.h" // 确保包含 int32_t 等类型定义

#define true 1
#define false 0
#define FS 100
#define BUFFER_SIZE  (FS * 5) 
#define HR_FIFO_SIZE 7
#define MA4_SIZE  4 
#define HAMMING_SIZE  5
#define min(x,y) ((x) < (y) ? (x) : (y))

/**
 * @brief 核心算法：计算心率和血氧
 * (原名 maxim_heart_rate_and_oxygen_saturation)
 */
void MAX30102_Algorithm_Process(uint32_t *pun_ir_buffer, int32_t n_ir_buffer_length, uint32_t *pun_red_buffer, int32_t *pn_spo2, int8_t *pch_spo2_valid, int32_t *pn_heart_rate, int8_t *pch_hr_valid);

// 辅助函数声明
void MAX30102_Algorithm_FindPeaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_size, int32_t n_min_height, int32_t n_min_distance, int32_t n_max_num);
void MAX30102_Algorithm_PeaksAboveMinHeight(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_size, int32_t n_min_height);
void MAX30102_Algorithm_RemoveClosePeaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_min_distance);
void MAX30102_Algorithm_SortAscend(int32_t *pn_x, int32_t n_size);
void MAX30102_Algorithm_SortIndicesDescend(int32_t *pn_x, int32_t *pn_indx, int32_t n_size);

#endif /* ALGORITHM_H_ */
