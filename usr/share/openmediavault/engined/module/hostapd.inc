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
require_once("openmediavault/module.inc");
require_once("openmediavault/config.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/util.inc");
require_once("openmediavault/initscript.inc");

class OMVModuleHostapd extends OMVModuleServiceAbstract
    implements OMVINotifyListener, OMVIModuleServiceStatus {

    /**
     * Get the module name.
     */
    public function getName() {
        return "hostapd";
    }

    public function applyConfig() {
        $cmd = "export LANG=C; omv-mkconf hostapd 2>&1";
		if (0 !== $this->exec($cmd, $output)) {
			throw new OMVException(OMVErrorMsg::E_EXEC_FAILED,
			  $cmd, implode("\n", $output));
		}
    }

	/**
	 * Stop the managed service.
	 */
	public function stopService() {
		$initd = new OMVSysVInitScript("hostapd", FALSE);
		$initd->exec();
	}

	/**
	 * Start the managed service.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	public function startService() {
		global $xmlConfig;
		$xpath = "//services/hostapd";
		$object = $xmlConfig->get($xpath);
		if (is_null($object)) {
			throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
			  $xpath);
		}
		if (FALSE === boolvalEx($object['enable']))
			return;
		$initd = new OMVSysVInitScript("hostapd", TRUE);
		$initd->exec();
	}

    /**
     * Get the module status.
     */
    public function getStatus() {
        global $xmlConfig;

        // Get the configuration object
        $xpath  = "//services/hostapd";
        $object = $xmlConfig->get($xpath);
        if (is_null($object))
            throw new OMVException(OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED, $xpath);

        $initd = new OMVSysVInitScript("hostapd");

        return array(
            "name"    => $this->getName(),
            "title"   => gettext("hostapd"),
            "enabled" => ! empty($object['enable']),
            "running" => ($initd->status() == 0)
        );
    }

    /**
     * Bind listeners.
     */
    function bindListeners(OMVNotifyDispatcher $dispatcher) {
        /* Update service if configuration has been modified */
        $dispatcher->addListener(
            OMV_NOTIFY_MODIFY,
            "org.openmediavault.services.hostapd",
            array($this, "setDirty"), 1);
    }

}

// Register module
$moduleMgr = &OMVModuleMgr::getInstance();
$moduleMgr->registerModule(new OMVModuleHostapd());

?>
