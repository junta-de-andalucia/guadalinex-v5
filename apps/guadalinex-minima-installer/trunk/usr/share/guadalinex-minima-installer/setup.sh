#!/bin/bash

function neterror() {

$DIALOG --aspect 15 --cr-wrap --title "Error de conexión" --trim \
--backtitle "GUADALINEX Mínima. Error de conexión." \
--yesno "No se pudo descargar la configuración del instalador.
	 La instalación no ha sido completada ¿ Desea completarla la
	 próxima vez que se inicie el sistema ?" 0 0

return $?

}

function cancelled() {

$DIALOG --aspect 15 --cr-wrap --title "Cancelar instalación" --trim \
--backtitle "GUADALINEX Mínima. Cancelación." \
--yesno "Ha cancelado la instalación, por lo tanto la instalación no ha sido
         completada ¿ Desea completarla la
	 próxima vez que se inicie el sistema ?" 0 0

return $?

}

export LANG=es_ES.UTF-8
VERSION="V5"
DIALOG="/usr/bin/dialog"
INDEXURL="http://www.guadalinex.org/distro/$VERSION/perfiles/index"

# show index download url confirmation
$DIALOG --aspect 15 --cr-wrap --nocancel --title "Selección del perfil" \
--backtitle "GUADALINEX. Mínima. Descarga de la lista de perfiles." \
--trim --inputbox "La lista de perfiles disponibles se descargará de esta URL. \
Puede cambiarla si lo desea:" 0 100 $INDEXURL 2> /tmp/gmi-url

# reset index download url from the inputbox
INDEXURL=$(cat /tmp/gmi-url)

# download index file
test -f /etc/gmi/index && rm /etc/gmi/index
wget $INDEXURL -O /etc/gmi/index

# show any error if there was any
if [ $? -ne 0 ]
then
	neterror
	if [ $? -eq 0 ]
	then
		exit 0	
	else
		rm $0
		exit 0
	fi
fi

# launch menu app
/usr/sbin/gmi
gmiexit=$?

# remove myself (/sbin/setup.sh) if apt-get install was launched
if [ $gmiexit -eq 0 ]
then
        $DIALOG --aspect 15 --cr-wrap --title "Reinicio del sistema" \
        --backtitle "GUADALINEX Mínima. Reinicio del sistema." \
        --msgbox "Se procederá a reinicar el sistema para terminar \
        la instalación." 6 50
        echo "Reiniciando el sistema para acabar la instalación..."
        sleep 5 && reboot &
	rm $0
	exit 0
fi

# there was any error
if [ $gmiexit -eq 69 ]
then
        $DIALOG --aspect 15 --cr-wrap --title "Errores en la instalación" --trim \
	--backtitle "GUADALINEX Mínima. Errores en la instalación." \
        --msgbox "Hubo algún problema durante la instalación de paquetes,
		 revise el fichero '/var/log/gmi.log' para obtener más
		 información." 0 0
	rm $0
	exit 1
fi

# no option chosen or cancel pressed
if [[ $gmiexit -eq 1 || $gmiexit -eq 255 ]]
then
	cancelled
        if [ $? -eq 0 ]
        then
                exit 0
        else
                rm $0
                exit 0
        fi	
fi
