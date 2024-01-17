################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
libraries/ThirdParty/lwip/test/unit/tcp/tcp_helper.obj: D:/Projects/ANAKS/librarys/ThirdParty/lwip/test/unit/tcp/tcp_helper.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/HalCoGen/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/Core/Inc" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/user/Inc" --include_path="D:/Projects/ANAKS/librarys/019-library-results/results/Inc" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/lwip/prot" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/netif" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/lwip" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/compat/posix" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="libraries/ThirdParty/lwip/test/unit/tcp/$(basename $(<F)).d_raw" --obj_directory="libraries/ThirdParty/lwip/test/unit/tcp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

libraries/ThirdParty/lwip/test/unit/tcp/test_tcp.obj: D:/Projects/ANAKS/librarys/ThirdParty/lwip/test/unit/tcp/test_tcp.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/HalCoGen/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/Core/Inc" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/user/Inc" --include_path="D:/Projects/ANAKS/librarys/019-library-results/results/Inc" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/lwip/prot" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/netif" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/lwip" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/compat/posix" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="libraries/ThirdParty/lwip/test/unit/tcp/$(basename $(<F)).d_raw" --obj_directory="libraries/ThirdParty/lwip/test/unit/tcp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

libraries/ThirdParty/lwip/test/unit/tcp/test_tcp_oos.obj: D:/Projects/ANAKS/librarys/ThirdParty/lwip/test/unit/tcp/test_tcp_oos.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/HalCoGen/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/Core/Inc" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/user/Inc" --include_path="D:/Projects/ANAKS/librarys/019-library-results/results/Inc" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/lwip/prot" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/netif" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/lwip" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/compat/posix" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="libraries/ThirdParty/lwip/test/unit/tcp/$(basename $(<F)).d_raw" --obj_directory="libraries/ThirdParty/lwip/test/unit/tcp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

libraries/ThirdParty/lwip/test/unit/tcp/test_tcp_state.obj: D:/Projects/ANAKS/librarys/ThirdParty/lwip/test/unit/tcp/test_tcp_state.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="C:/ti/ccs1100/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/HalCoGen/include" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/Core/Inc" --include_path="D:/Projects/ANAKS/Projects/Software/ES/SOM_TMS570LS3137/SOM_TMS570LS3137_V1.0/user/Inc" --include_path="D:/Projects/ANAKS/librarys/019-library-results/results/Inc" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/lwip/prot" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/netif" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/lwip" --include_path="D:/Projects/ANAKS/librarys/ThirdParty/lwip/src/include/compat/posix" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="libraries/ThirdParty/lwip/test/unit/tcp/$(basename $(<F)).d_raw" --obj_directory="libraries/ThirdParty/lwip/test/unit/tcp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


