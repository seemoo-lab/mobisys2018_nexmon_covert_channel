#!/system/bin/sh

echo "waiting 30 seconds"
sleep 30
echo "sending three test acks"
matlabframe_nonht_0.sh
nexutil -g729
echo "waiting 30 seconds"
sleep 30
echo "starting experiments"
matlabframe_nonht_0.sh
nexutil -g726
sleep 360
echo "nonht_0 done"
matlabframe_nonht_1.sh
nexutil -g726
sleep 360
echo "nonht_1 done"
matlabframe_nonht_2.sh
nexutil -g726
sleep 360
echo "nonht_2 done"
matlabframe_nonht_3.sh
nexutil -g726
sleep 360
echo "nonht_3 done"
matlabframe_nonht_4.sh
nexutil -g726
sleep 360
echo "nonht_4 done"
matlabframe_nonht_5.sh
nexutil -g726
sleep 360
echo "nonht_5 done"
matlabframe_nonht_6.sh
nexutil -g726
sleep 360
echo "nonht_6 done"
matlabframe_nonht_7.sh
nexutil -g726
sleep 360
echo "nonht_7 done"
matlabframe_ht_0.sh
nexutil -g726
sleep 360
echo "ht_0 done"
matlabframe_ht_1.sh
nexutil -g726
sleep 360
echo "ht_1 done"
matlabframe_ht_2.sh
nexutil -g726
sleep 360
echo "ht_2 done"
matlabframe_ht_3.sh
nexutil -g726
sleep 360
echo "ht_3 done"
matlabframe_ht_4.sh
nexutil -g726
sleep 360
echo "ht_4 done"
matlabframe_ht_5.sh
nexutil -g726
sleep 360
echo "ht_5 done"
matlabframe_ht_6.sh
nexutil -g726
sleep 360
echo "ht_6 done"
matlabframe_ht_7.sh
nexutil -g726
sleep 360
echo "ht_7 done"
