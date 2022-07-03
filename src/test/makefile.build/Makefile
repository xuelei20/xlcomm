# +++++++++++++++++++++++++++++ 这里两行东西，二选一 +++++++++++++++++++++++++++++ #

# 如果是动态库，包含这个文件
#include $(XLCOMM_ROOT_PATH)/automation/linux/makefile.dep/makefile.template.common.dll.dep

# 如果是可执行程序，包含这个文件
include $(XLCOMM_ROOT_PATH)/automation/linux/makefile.dep/makefile.template.common.exe.dep

# ----------------------------- 上面两行东西，二选一 ----------------------------- #


# +++++++++++++++++++++++++++++ 这条线下面的内容，一般需要修改一下 +++++++++++++++++++++++++++++ #

# 当前工程的目录列表( 把需要编译的目录，添加到这个文件中去 )
include Makefile.this.project.dep

# +++++++++++++++++ 这个放到最后，一般不用动 +++++++++++++++++ #

include $(XLCOMM_ROOT_PATH)/automation/linux/makefile.dep/makefile.template.final.dep

# ----------------- 这个放到最后，一般不用动 ----------------- #
