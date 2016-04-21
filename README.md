# joshtool

1. Init this project under Android AOSP

   $ cd $AOSP_ROOT

   $ mkdir vendor/mumu

   and sync this project to vendor/mumu
   
2. Include this project

   in your device chip provider make file (ex: device/qcom/msm8916/msm8916.mk)

   add following lines
   
   +# Porting
   
   +$(call inherit-product-if-exists, vendor/mumu/mumu.mk)
   
   +

3. Modify SELinux policy to permissive
  
   find the function sel_write_enforce in kernel/security/selinux/selinuxfs.c

   set new_value = 0; before "if (new_value != selinux_enforcing) {"


4. Build
   
   $ m -j8


