setenv PM_SRC_DIR `pwd`
set path = ($path $PM_SRC_DIR/bin)
cp $PM_SRC_DIR/common/odbc.ini $HOME/.odbc.ini
head -n 3 $HOME/.odbc.ini
