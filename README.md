# Yugr19-flowmon
A OpenWrt Plugin. Rely on nf_conntrack to calculate real-time network speed.
本插件旨在解决 MT798X、IPQ60XX、IPQ53XX、IPQ50XX、IPQ807X（Cortex‑A53）等低性能处理器平台在开启硬件加速后，bandix 等插件无法准确反映设备实时网络速率的问题。Flowmon 基于 kmod‑nf‑conntrack 实现，是一款具备高效率与高兼容性的网络速率统计工具。
