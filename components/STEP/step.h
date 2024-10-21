//
// Created by li'xiao on 2024/9/26.
//

#ifndef STEP_H
#define STEP_H

#define		ABS(x)		((x) > 0 ? (x) : -(x))

#include "usart.h"

typedef struct {
    uint32_t speed;
    uint32_t angle;
    uint32_t num;
    uint32_t dir;
} StepMotor_t;

extern StepMotor_t stepMotor;

extern StepMotor_t stepMotor_1;
extern StepMotor_t stepMotor_2;
extern StepMotor_t stepMotor_3;
extern StepMotor_t stepMotor_4;

typedef enum {
    S_VER   = 0,      /* 读取固件版本和对应的硬件版本 */
    S_RL    = 1,      /* 读取读取相电阻和相电感 */
    S_PID   = 2,      /* 读取PID参数 */
    S_ORG   = 3,      /* 读取回零参数 */
    S_VBUS  = 4,      /* 读取总线电压 */
    S_CBUS  = 5,      /* 读取总线电流 */
    S_CPHA  = 6,      /* 读取相电流 */
    S_ENC   = 7,      /* 读取编码器原始值 */
    S_CPUL  = 8,      /* 读取实时脉冲数（根据实时位置计算得到的脉冲数） */
    S_ENCL  = 9,      /* 读取经过线性化校准后的编码器值 */
    S_TPUL  = 10,     /* 读取输入脉冲数 */
    S_TPOS  = 11,     /* 读取电机目标位置 */
    S_OPOS  = 12,     /* 读取电机实时设定的目标位置（开环模式的实时位置） */
    S_VEL   = 13,     /* 读取电机实时转速 */
    S_CPOS  = 14,     /* 读取电机实时位置（基于角度编码器累加的电机实时位置） */
    S_PERR  = 15,     /* 读取电机位置误差 */
    S_TEMP  = 16,     /* 读取电机实时温度 */
    S_SFLAG = 17,     /* 读取状态标志位 */
    S_OFLAG = 18,     /* 读取回零状态标志位 */
    S_Conf  = 19,     /* 读取驱动参数 */
    S_State = 20,     /* 读取系统状态参数 */
  }SysParams_t;

void Reset_CurPos_To_Zero(uint8_t addr); // 将当前位置清零
void Reset_Clog_Pro(uint8_t addr); // 解除堵转保护
void Read_Sys_Params(uint8_t addr, SysParams_t s); // 读取参数
void Modify_Ctrl_Mode(uint8_t addr, bool svF, uint8_t ctrl_mode); // 发送命令切换开环/闭环控制模式
void En_Control(uint8_t addr, bool state, uint8_t snF); // 电机使能控制
void Torque_Control(uint8_t addr, uint8_t sign, uint16_t t_ramp, uint16_t torque, uint8_t snF); // 力矩模式控制
void Velocity_Control(uint8_t addr, uint8_t dir, uint16_t v_ramp, float velocity, uint8_t snF); // 速度模式控制
void Bypass_Position_LV_Control(uint8_t addr, uint8_t dir, float velocity, float position, uint8_t raf, uint8_t snF); // 直通限速位置模式控制
void Traj_Position_Control(uint8_t addr, uint8_t dir, uint16_t acc, uint16_t dec, float velocity, float position, uint8_t raf, uint8_t snF); // 梯形曲线加减速位置模式控制
void Stop_Now(uint8_t addr, uint8_t snF); // 让电机立即停止运动
void Synchronous_motion(uint8_t addr); // 触发多机同步开始运动
void Origin_Set_O(uint8_t addr, bool svF); // 设置单圈回零的零点位置
void Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF); // 修改回零参数
void Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF); // 发送命令触发回零
void Origin_Interrupt(uint8_t addr); // 强制中断并退出回零
uint32_t stepNumStrToData(uint32_t stepNumStr);
uint32_t stepDirStrToData(uint32_t stepDirStr);
void pulseOutput(uint32_t stepDir, uint32_t stepNum, uint32_t vel, uint32_t angle);
void stepRotate_1_2(uint32_t stepDir, uint32_t vel, uint32_t angle);
void stepRotate_3_4(uint32_t stepDir, uint32_t vel, uint32_t angle);
void stepRotate_5_6(uint32_t stepDir, uint32_t vel, uint32_t angle);
void stepRotate_7_8(uint32_t stepDir, uint32_t vel, uint32_t angle);
void stepRotate_1(uint32_t stepDir, uint32_t vel, uint32_t angle);
void stepRotate_2(uint32_t stepDir, uint32_t vel, uint32_t angle);
void stepRotate_3(uint32_t stepDir, uint32_t vel, uint32_t angle);
void stepRotate_4(uint32_t stepDir, uint32_t vel, uint32_t angle);
void stepRotate_5(uint32_t stepDir, uint32_t vel, uint32_t angle);
void stepRotate_6(uint32_t stepDir, uint32_t vel, uint32_t angle);
void stepRotate_7(uint32_t stepDir, uint32_t vel, uint32_t angle);
void stepRotate_8(uint32_t stepDir, uint32_t vel, uint32_t angle);
void pulseOutput_single(uint32_t stepDir, uint32_t vel, uint32_t angle, uint32_t stepNum);
void stepDataInit(void);
void stepDataInit_1(void);
void stepDataInit_2(void);
void stepDataInit_3(void);
void stepDataInit_4(void);



#endif //STEP_H
