################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
user/Src/%.obj: ../user/Src/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -Ooff --opt_for_speed=1 --include_path="C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/HalCoGen/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/Core/Inc" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/user/Inc" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include" --include_path="D:/Projects/ANAKS/librarys/019-library-results/results/Inc" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/netif" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/lwip/prot" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/lwip/priv" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/lwip/apps" -g --c99 --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="user/Src/$(basename $(<F)).d_raw" --obj_directory="user/Src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


