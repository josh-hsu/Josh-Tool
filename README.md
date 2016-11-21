# Josh Tool

Josh-Tool is an Android based native game automation framework for developers. We bundled all you need for game automation such as color on screen point, touch on specific point, receive events from keys and so on.

## Use binary directly

You can use prebuilt binary if you have your own phone rooted or unlocked.

* Go to joshtool folder and download joshtool binary
* put it in your /system/bin folder, You might need to remount your system partition

```shell
    adb shell
    su
    mount -o rw,remount /system
```

* Remember to set SEPolicy to permissive mode if necessary

## Setup the build

* Init this project under Android AOSP

```shell
    cd $AOSP_ROOT
    mkdir vendor/mumu
```
   and sync this project to vendor/mumu

* Include this project

```diff
   in your device chip provider make file (ex: device/qcom/msm8916/msm8916.mk)
   +# Porting
   +$(call inherit-product-if-exists, vendor/mumu/mumu.mk)
   +
```

* Modify SELinux policy to permissive
  
   find the function sel_write_enforce in kernel/security/selinux/selinuxfs.c

   set new_value = 0; before "if (new_value != selinux_enforcing) {"
```c
+  new_value = 0
   if (new_value != selinux_enforcing) {
       ...
   }
```

* Build
   
```shell
    m -j4
```


