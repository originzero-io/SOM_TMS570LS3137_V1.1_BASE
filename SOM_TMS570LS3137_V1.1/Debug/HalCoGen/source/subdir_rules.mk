################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
HalCoGen/source/%.obj: ../HalCoGen/source/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --opt_for_speed=1 --include_path="D:/Projects/001-ANAKS/Projects/Software/ES/SOM_TMS570LS3137_V1.1_BASE/SOM_TMS570LS3137_V1.1" --include_path="C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="D:/Projects/001-ANAKS/Projects/Software/ES/SOM_TMS570LS3137_V1.1_BASE/SOM_TMS570LS3137_V1.1/HalCoGen/include" --include_path="D:/Projects/001-ANAKS/Projects/Software/ES/SOM_TMS570LS3137_V1.1_BASE/SOM_TMS570LS3137_V1.1/Core/Inc" --include_path="D:/Projects/001-ANAKS/Projects/Software/ES/SOM_TMS570LS3137_V1.1_BASE/SOM_TMS570LS3137_V1.1/LwIP/src/include" --include_path="D:/Projects/001-ANAKS/Projects/Software/ES/SOM_TMS570LS3137_V1.1_BASE/SOM_TMS570LS3137_V1.1/LwIP/system" --include_path="D:/Projects/001-ANAKS/librarys/019-library-results/results/Inc" -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="HalCoGen/source/$(basename $(<F)).d_raw" --obj_directory="HalCoGen/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

HalCoGen/source/%.obj: ../HalCoGen/source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --opt_for_speed=1 --include_path="D:/Projects/001-ANAKS/Projects/Software/ES/SOM_TMS570LS3137_V1.1_BASE/SOM_TMS570LS3137_V1.1" --include_path="C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="D:/Projects/001-ANAKS/Projects/Software/ES/SOM_TMS570LS3137_V1.1_BASE/SOM_TMS570LS3137_V1.1/HalCoGen/include" --include_path="D:/Projects/001-ANAKS/Projects/Software/ES/SOM_TMS570LS3137_V1.1_BASE/SOM_TMS570LS3137_V1.1/Core/Inc" --include_path="D:/Projects/001-ANAKS/Projects/Software/ES/SOM_TMS570LS3137_V1.1_BASE/SOM_TMS570LS3137_V1.1/LwIP/src/include" --include_path="D:/Projects/001-ANAKS/Projects/Software/ES/SOM_TMS570LS3137_V1.1_BASE/SOM_TMS570LS3137_V1.1/LwIP/system" --include_path="D:/Projects/001-ANAKS/librarys/019-library-results/results/Inc" -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="HalCoGen/source/$(basename $(<F)).d_raw" --obj_directory="HalCoGen/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


