#!/bin/bash


RESOURCEPATH="resources"

echo -n "" > resources-inc.h
echo -n "" > resources-defs.h
for n in `find "$RESOURCEPATH" -name '*.png'` ; do
    name=`basename "$n" | sed 's/\.png$//g'`
    hname="$name.h"
    convert "$n" "temp-$$.h"
    cat temp-$$.h | sed "s/MagickImage\[\]/MagickImage_$name[]/g" > "$RESOURCEPATH/$hname"
    rm -rf "temp-$$.h"
    echo "#include \"$RESOURCEPATH/$hname\"" >> resources-inc.h
    echo "    {\"$name\", MagickImage_$name, sizeof(MagickImage_$name)}," >> resources-defs.h
    echo "$n converted to $hname"
done

echo "/*" > resources.h
echo "#########################################" >> resources.h
echo "###       Please DON'T EDIT!!!        ###" >> resources.h
echo "### generated file by genresources.sh ###" >> resources.h
echo "###       Please DON'T EDIT!!!        ###" >> resources.h
echo "#########################################" >> resources.h
echo "*/" >> resources.h
echo "" >> resources.h
cat resources-inc.h >> resources.h
echo "" >> resources.h
echo "typedef struct {" >> resources.h
echo "    char*               name;" >> resources.h
echo "    unsigned char*      data;" >> resources.h
echo "    size_t              size;" >> resources.h
echo "} resources_t;" >> resources.h
echo "" >> resources.h
echo "resources_t resources[] = {" >> resources.h
cat resources-defs.h >> resources.h
echo "    {NULL, NULL, 0}" >> resources.h
echo "};" >> resources.h
echo "" >> resources.h
rm -rf resources-inc.h
rm -rf resources-defs.h




