#!/bin/bash
# $1 - text_vmaddr, $2 - text_offset, $3 - text_size, $4 - var_offset, $5 - program name, $6 - target bin > hack()
hack()
{
  global_var_offset=$4
  global_var_offset=$(($global_var_offset + ${offsets_arr[$i]}))
  main_off=$(($2 + ${offsets_arr[$i]}))
  size=$3
  echo "main_off:$main_off size:$size"
  dd if="$6" ibs=$main_off skip=1 of=${TMP_FILE}.1
  dd if=${TMP_FILE}.1 ibs=$size count=1 of=$TMP_FILE
  checksum=`"$CPVERIFY" -mk "$TMP_FILE" -alg GR3411_2012_256` || return $?
  rm $TMP_FILE ${TMP_FILE}.1
  echo $EMBED_CHECKSUM $size $global_var_offset $checksum "$5" 
  "$EMBED_CHECKSUM" $size $global_var_offset $checksum "$5"
}

MY_DIR=`dirname "$0"`
CPVERIFY=${CPVERIFY:-$MY_DIR/cpverify}
EMBED_CHECKSUM=${EMBED_CHECKSUM:-$MY_DIR/EmbedChecksum}
TMP_FILE=`mktemp /tmp/hack_exe.XXXXXX`
#cp "$1" "$1".bak
otool_detect_many_archs_output=`otool -v -f "$1"`
if test "$otool_detect_many_archs_output" != ""; then
  archs_number=`otool -v -f "$1" | grep nfat_arch | awk '{print $2}'`
  archs=`otool -v -f "$1" | grep architecture | awk '{print $2}'`
  offsets=`otool -v -f "$1" | grep offset | awk '{print $2}'`
  i=0
  while [ $i -lt $archs_number ]; do
    archs_arr[$i]=`echo $archs | awk -v j=$(($i+1)) '{print $j}'`
    offsets_arr[$i]=`echo $offsets | awk -v j=$(($i+1)) '{print $j}'`
    i=$(($i+1))
  done
else
  archs_arr[0]=`lipo -info "$1" | awk -F": " '{print $3}'`
  offsets_arr[0]=0
  archs_number=1
fi

i=0
while [ $i -lt $archs_number ]; do
  arch=${archs_arr[$i]}

  var_offset=`nm -arch $arch "$1"|awk '/[D|S].*_'$2'/{print "0x"$1}'`
  if test -z "$var_offset"; then echo "nm can not find _program_control_sum symbol" && exit 1; fi

  if test -z "$3"; then 
    target_bin="$1"
  else 
    target_bin="$3"
  fi

hack  `otool -l -arch $arch "$target_bin"|awk -v var_vmaddr=$var_offset '
function hex_to_dec(x,s)
{
    x=tolower(x)
    gsub("[[:blank:]]*","",x)
    if (substr(x,1,2) != "0x")
	return x
    s=0
    for (i=3;i<=length(x);i++)
	s=(s*16)+h[substr(x,i,1)]
    return s
}

BEGIN {
    lc_num = -1;
    h["0"]=0
    h["1"]=1
    h["2"]=2
    h["3"]=3
    h["4"]=4
    h["5"]=5
    h["6"]=6
    h["7"]=7
    h["8"]=8
    h["9"]=9
    h["a"]=10
    h["b"]=11
    h["c"]=12
    h["d"]=13
    h["e"]=14
    h["f"]=15
    var_vmaddr=hex_to_dec(var_vmaddr)
}

/Load command/ {
    sub("^.*Load command[[:blank:]]*","");
    lc_num=$0;
    segname="";
    sectname="";
    first=0;
}
/Section/{
    sectname="";
}
/segname/ {
    sub("^.*segname[[:blank:]]*","");
    segname=$0;
}
/sectname/ {
    sub("^.*sectname[[:blank:]]*","");
    sectname=$0;
}

/vmaddr/ {
    sub("^.*vmaddr[[:blank:]]*","");
    if (segname == "__TEXT")
    { 
	text_vmaddr=hex_to_dec($0);
    }
    if (segname == "__DATA")
    {
	data_vmaddr=hex_to_dec($0);
    }
}
/fileoff/ {
    sub("^.*fileoff[[:blank:]]*","");
    if (segname == "__TEXT")
	text_fileoff=$0;
    if (segname == "__DATA")
	data_fileoff=$0;
}
/offset/ {
    sub("^.*offset[[:blank:]]*","");
    if (segname == "__TEXT" && sectname=="__text")
	text_offset=$0;
}
/[^a-z]size/ {
    sub("^.*size[[:blank:]]*","");
    if (segname == "__TEXT" && sectname=="__text")
	text_size=hex_to_dec($0);
}
END {
    var_offset=var_vmaddr-data_vmaddr+data_fileoff
    printf "%d %d %d %d",text_vmaddr,text_offset,text_size,var_offset
}'` "$1" "$target_bin" || exit $?
  i=$(($i+1))
done
