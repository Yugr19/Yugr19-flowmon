# Yugr19-flowmon
A OpenWrt Plugin. Rely on nf_conntrack to calculate real-time network speed.
本插件旨在解决IPQ6000,MT7921等使用A53的低性能处理器在启用硬件加速后bandix等Plugin无法准确反映设备实时网络速率问题。Flowmon依赖于kmod-nf-conntrack，从而实现高效率高兼容性的的网络速率统计工具。
