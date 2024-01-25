################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../HalCoGen/source/sys_link.cmd 

ASM_SRCS += \
../HalCoGen/source/dabort.asm \
../HalCoGen/source/os_portasm.asm \
../HalCoGen/source/sys_core.asm \
../HalCoGen/source/sys_intvecs.asm \
../HalCoGen/source/sys_mpu.asm \
../HalCoGen/source/sys_pmu.asm 

C_SRCS += \
../HalCoGen/source/dcc.c \
../HalCoGen/source/emac.c \
../HalCoGen/source/errata_SSWF021_45.c \
../HalCoGen/source/esm.c \
../HalCoGen/source/mdio.c \
../HalCoGen/source/notification.c \
../HalCoGen/source/os_croutine.c \
../HalCoGen/source/os_event_groups.c \
../HalCoGen/source/os_heap.c \
../HalCoGen/source/os_list.c \
../HalCoGen/source/os_mpu_wrappers.c \
../HalCoGen/source/os_port.c \
../HalCoGen/source/os_queue.c \
../HalCoGen/source/os_tasks.c \
../HalCoGen/source/os_timer.c \
../HalCoGen/source/phy_dp83640.c \
../HalCoGen/source/pinmux.c \
../HalCoGen/source/sci.c \
../HalCoGen/source/sys_dma.c \
../HalCoGen/source/sys_main.c \
../HalCoGen/source/sys_pcr.c \
../HalCoGen/source/sys_phantom.c \
../HalCoGen/source/sys_pmm.c \
../HalCoGen/source/sys_selftest.c \
../HalCoGen/source/sys_startup.c \
../HalCoGen/source/sys_vim.c \
../HalCoGen/source/system.c 

C_DEPS += \
./HalCoGen/source/dcc.d \
./HalCoGen/source/emac.d \
./HalCoGen/source/errata_SSWF021_45.d \
./HalCoGen/source/esm.d \
./HalCoGen/source/mdio.d \
./HalCoGen/source/notification.d \
./HalCoGen/source/os_croutine.d \
./HalCoGen/source/os_event_groups.d \
./HalCoGen/source/os_heap.d \
./HalCoGen/source/os_list.d \
./HalCoGen/source/os_mpu_wrappers.d \
./HalCoGen/source/os_port.d \
./HalCoGen/source/os_queue.d \
./HalCoGen/source/os_tasks.d \
./HalCoGen/source/os_timer.d \
./HalCoGen/source/phy_dp83640.d \
./HalCoGen/source/pinmux.d \
./HalCoGen/source/sci.d \
./HalCoGen/source/sys_dma.d \
./HalCoGen/source/sys_main.d \
./HalCoGen/source/sys_pcr.d \
./HalCoGen/source/sys_phantom.d \
./HalCoGen/source/sys_pmm.d \
./HalCoGen/source/sys_selftest.d \
./HalCoGen/source/sys_startup.d \
./HalCoGen/source/sys_vim.d \
./HalCoGen/source/system.d 

OBJS += \
./HalCoGen/source/dabort.obj \
./HalCoGen/source/dcc.obj \
./HalCoGen/source/emac.obj \
./HalCoGen/source/errata_SSWF021_45.obj \
./HalCoGen/source/esm.obj \
./HalCoGen/source/mdio.obj \
./HalCoGen/source/notification.obj \
./HalCoGen/source/os_croutine.obj \
./HalCoGen/source/os_event_groups.obj \
./HalCoGen/source/os_heap.obj \
./HalCoGen/source/os_list.obj \
./HalCoGen/source/os_mpu_wrappers.obj \
./HalCoGen/source/os_port.obj \
./HalCoGen/source/os_portasm.obj \
./HalCoGen/source/os_queue.obj \
./HalCoGen/source/os_tasks.obj \
./HalCoGen/source/os_timer.obj \
./HalCoGen/source/phy_dp83640.obj \
./HalCoGen/source/pinmux.obj \
./HalCoGen/source/sci.obj \
./HalCoGen/source/sys_core.obj \
./HalCoGen/source/sys_dma.obj \
./HalCoGen/source/sys_intvecs.obj \
./HalCoGen/source/sys_main.obj \
./HalCoGen/source/sys_mpu.obj \
./HalCoGen/source/sys_pcr.obj \
./HalCoGen/source/sys_phantom.obj \
./HalCoGen/source/sys_pmm.obj \
./HalCoGen/source/sys_pmu.obj \
./HalCoGen/source/sys_selftest.obj \
./HalCoGen/source/sys_startup.obj \
./HalCoGen/source/sys_vim.obj \
./HalCoGen/source/system.obj 

ASM_DEPS += \
./HalCoGen/source/dabort.d \
./HalCoGen/source/os_portasm.d \
./HalCoGen/source/sys_core.d \
./HalCoGen/source/sys_intvecs.d \
./HalCoGen/source/sys_mpu.d \
./HalCoGen/source/sys_pmu.d 

OBJS__QUOTED += \
"HalCoGen\source\dabort.obj" \
"HalCoGen\source\dcc.obj" \
"HalCoGen\source\emac.obj" \
"HalCoGen\source\errata_SSWF021_45.obj" \
"HalCoGen\source\esm.obj" \
"HalCoGen\source\mdio.obj" \
"HalCoGen\source\notification.obj" \
"HalCoGen\source\os_croutine.obj" \
"HalCoGen\source\os_event_groups.obj" \
"HalCoGen\source\os_heap.obj" \
"HalCoGen\source\os_list.obj" \
"HalCoGen\source\os_mpu_wrappers.obj" \
"HalCoGen\source\os_port.obj" \
"HalCoGen\source\os_portasm.obj" \
"HalCoGen\source\os_queue.obj" \
"HalCoGen\source\os_tasks.obj" \
"HalCoGen\source\os_timer.obj" \
"HalCoGen\source\phy_dp83640.obj" \
"HalCoGen\source\pinmux.obj" \
"HalCoGen\source\sci.obj" \
"HalCoGen\source\sys_core.obj" \
"HalCoGen\source\sys_dma.obj" \
"HalCoGen\source\sys_intvecs.obj" \
"HalCoGen\source\sys_main.obj" \
"HalCoGen\source\sys_mpu.obj" \
"HalCoGen\source\sys_pcr.obj" \
"HalCoGen\source\sys_phantom.obj" \
"HalCoGen\source\sys_pmm.obj" \
"HalCoGen\source\sys_pmu.obj" \
"HalCoGen\source\sys_selftest.obj" \
"HalCoGen\source\sys_startup.obj" \
"HalCoGen\source\sys_vim.obj" \
"HalCoGen\source\system.obj" 

C_DEPS__QUOTED += \
"HalCoGen\source\dcc.d" \
"HalCoGen\source\emac.d" \
"HalCoGen\source\errata_SSWF021_45.d" \
"HalCoGen\source\esm.d" \
"HalCoGen\source\mdio.d" \
"HalCoGen\source\notification.d" \
"HalCoGen\source\os_croutine.d" \
"HalCoGen\source\os_event_groups.d" \
"HalCoGen\source\os_heap.d" \
"HalCoGen\source\os_list.d" \
"HalCoGen\source\os_mpu_wrappers.d" \
"HalCoGen\source\os_port.d" \
"HalCoGen\source\os_queue.d" \
"HalCoGen\source\os_tasks.d" \
"HalCoGen\source\os_timer.d" \
"HalCoGen\source\phy_dp83640.d" \
"HalCoGen\source\pinmux.d" \
"HalCoGen\source\sci.d" \
"HalCoGen\source\sys_dma.d" \
"HalCoGen\source\sys_main.d" \
"HalCoGen\source\sys_pcr.d" \
"HalCoGen\source\sys_phantom.d" \
"HalCoGen\source\sys_pmm.d" \
"HalCoGen\source\sys_selftest.d" \
"HalCoGen\source\sys_startup.d" \
"HalCoGen\source\sys_vim.d" \
"HalCoGen\source\system.d" 

ASM_DEPS__QUOTED += \
"HalCoGen\source\dabort.d" \
"HalCoGen\source\os_portasm.d" \
"HalCoGen\source\sys_core.d" \
"HalCoGen\source\sys_intvecs.d" \
"HalCoGen\source\sys_mpu.d" \
"HalCoGen\source\sys_pmu.d" 

ASM_SRCS__QUOTED += \
"../HalCoGen/source/dabort.asm" \
"../HalCoGen/source/os_portasm.asm" \
"../HalCoGen/source/sys_core.asm" \
"../HalCoGen/source/sys_intvecs.asm" \
"../HalCoGen/source/sys_mpu.asm" \
"../HalCoGen/source/sys_pmu.asm" 

C_SRCS__QUOTED += \
"../HalCoGen/source/dcc.c" \
"../HalCoGen/source/emac.c" \
"../HalCoGen/source/errata_SSWF021_45.c" \
"../HalCoGen/source/esm.c" \
"../HalCoGen/source/mdio.c" \
"../HalCoGen/source/notification.c" \
"../HalCoGen/source/os_croutine.c" \
"../HalCoGen/source/os_event_groups.c" \
"../HalCoGen/source/os_heap.c" \
"../HalCoGen/source/os_list.c" \
"../HalCoGen/source/os_mpu_wrappers.c" \
"../HalCoGen/source/os_port.c" \
"../HalCoGen/source/os_queue.c" \
"../HalCoGen/source/os_tasks.c" \
"../HalCoGen/source/os_timer.c" \
"../HalCoGen/source/phy_dp83640.c" \
"../HalCoGen/source/pinmux.c" \
"../HalCoGen/source/sci.c" \
"../HalCoGen/source/sys_dma.c" \
"../HalCoGen/source/sys_main.c" \
"../HalCoGen/source/sys_pcr.c" \
"../HalCoGen/source/sys_phantom.c" \
"../HalCoGen/source/sys_pmm.c" \
"../HalCoGen/source/sys_selftest.c" \
"../HalCoGen/source/sys_startup.c" \
"../HalCoGen/source/sys_vim.c" \
"../HalCoGen/source/system.c" 


