################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
lwip/test/unit/ip4/test_ip4.obj: D:/Projects/ANAKS/librarys/ThirdParty/lwip/test/unit/ip4/test_ip4.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/HalCoGen" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/HalCoGen/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/Core/Inc" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/user/Inc" --include_path="D:/Projects/ANAKS/librarys/024-library-w25q32jv_flash/w25q32jv_flash/Inc" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="lwip/test/unit/ip4/$(basename $(<F)).d_raw" --obj_directory="lwip/test/unit/ip4" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


