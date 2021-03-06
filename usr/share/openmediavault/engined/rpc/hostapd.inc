<?php
/**
 * @license     http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author      Simon Baur <sbausis@gmx.net>
 * @copyright   Copyright (c) 2015 Simon Baur
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
require_once("openmediavault/object.inc");
require_once("openmediavault/config.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/util.inc");
require_once("openmediavault/rpcservice.inc");
require_once("openmediavault/notify.inc");

class OMVRpcServiceHostapd extends OMVRpcServiceAbstract {

    /**
     * Get the RPC service name.
     */
    public function getName() {
        return "Hostapd";
    }

    /**
     * Initialize the RPC service.
     */
    public function initialize() {
        $this->registerMethod("getSettings");
        $this->registerMethod("setSettings");
		// add more Functions to your RPC-Service
    }

    /**
     * Get the settings configuration object.
     * @param params The method parameters.
     * @param context The context of the caller.
     * @return The requested configuration object.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     */
    public function getSettings($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Get the configuration object.
        $xpath = "//services/hostapd";
        $object = $xmlConfig->get($xpath);
        if(is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        // Modify the result object.
        $object['enable'] 		= boolval($object['enable']);
        $object['ssid'] 		= $object['ssid'];
        $object['passphrase'] 	= $object['passphrase'];

        // Remove useless fields from the result object.
        //unset($object['ddd']);

        return $object;
    }

    /**
     * Set the settings configuration object.
     * @param params The method parameters.
     * @param context The context of the caller.
     * @return The stored configuration object.
     * @throw E_CONFIG_GET_OBJECT_FAILED
     * @throw E_CONFIG_SET_OBJECT_FAILED
     */
    public function setSettings($params, $context) {
        global $xmlConfig;

        // Validate the RPC caller context.
        $this->validateMethodContext($context, array( "role" => OMV_ROLE_ADMINISTRATOR ));

        // Validate the parameters of the RPC service method.
        $this->validateMethodParams($params, '{
            "type"       : "object",
            "properties" : {
                "enable" 		: { "type" : "boolean" },
				"ssid" 			: { "type" : "string" },
				"passphrase" 	: { "type" : "string" }
            }
        }');

        // Get the existing configuration object.
        $xpath = "//services/hostapd";
        $oldObject = $xmlConfig->get($xpath);
        if(is_null($oldObject))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        // Update the configuration object.
        $object = array(
            "enable" 		=> array_boolval($params, 'enable'),
			"ssid" 			=> $params['ssid'],
			"passphrase" 	=> $params['passphrase'],
        );

        if(FALSE === $xmlConfig->replace("//services/hostapd", $object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED);

        // Notify configuration changes.
        $dispatcher = &OMVNotifyDispatcher::getInstance();
        $dispatcher->notify(OMV_NOTIFY_MODIFY, "org.openmediavault.services.hostapd", $object);

        // Return the configuration object.
        return $object;
    }

}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceHostapd());
