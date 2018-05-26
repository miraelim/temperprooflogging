alg_primary_obj=0x000B
alg_primary_key=0x0023
alg_create_obj=0x000B
alg_create_key=0x0008

file_input_data=key.txt
file_primary_key_ctx=primary.txt
file_unseal_key_pub=public.txt
file_unseal_key_priv=private.txt
file_unseal_key_ctx=unsealctx.txt
file_unseal_key_name=loadname.txt
file_unseal_output_data=unsealoutput.txt 


#tpm2_takeownership -c
#tpm2_createprimary -A p -g $alg_primary_obj -G $alg_primary_key -C $file_primary_key_ctx
#if [ $? != 0 ];then
#echo "createprimary fail, please check the environment or parameters!"
#exit 1
#fi

#tpm2_create -g $alg_create_obj -G $alg_create_key -o $file_unseal_key_pub -O $file_unseal_key_priv  -I $file_input_data -c $file_primary_key_ctx
#if [ $? != 0 ];then
#echo "create fail, please check the environment or parameters!"
#exit 1
#fi

#tpm2_load -c $file_primary_key_ctx  -u $file_unseal_key_pub  -r $file_unseal_key_priv -n $file_unseal_key_name -C $file_unseal_key_ctx
#if [ $? != 0 ];then
#echo "load fail, please check the environment or parameters!"
#exit 1
#fi
rm unsealoutput.txt

tpm2_unseal -c $file_unseal_key_ctx -o $file_unseal_output_data
if [ $? != 0 ];then
echo "unseal fail, please check the environment or parameters!"
exit 1
fi
