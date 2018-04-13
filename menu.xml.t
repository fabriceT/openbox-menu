<?xml version="1.0" encoding="UTF-8"?>
<openbox_menu xmlns="http://openbox.org/3.4/menu">
  <menu id="osmo-both" label="Agenda &amp; Tâches" execute="osmo-menu calendar tasks" />
  <menu id="info-menu" label="_Information" execute="lua ~/.config/openbox/info-menu" />
  <menu id="bookmarks-menu" label="_Bookmarks" execute="lua ~/.config/openbox/bookmarks" />
  <menu id="audacious" label="A_udacious" execute="lua ~/.config/openbox/audacious" />
  <menu id="xdg-session" label="Démarrage" execute="lua ~/.config/openbox/lxdg-session" />
  <menu id="arandr-profiles" label="Profiles écrans" execute="sh ~/.config/openbox/arandr_profiles.sh" />
<!--  <menu id="obmount" label="Disques" execute="python ~/.local/bin/obmount.py" /> -->
  <menu id="openbox-menu" label="_Openbox">
    <menu id="client-list-menu" />
    <menu id="arandr-profiles" />
    <menu id="xdg-session" />
    <item label="Reconfigurer">
      <action name="Reconfigure" />
    </item>
    <separator />
    <item label="_Quitter">
      <prompt>yes</prompt>
      <action name="Exit" />
    </item>
    <separator />
    <item label="Éte_indre">
      <action name="Execute">
        <command>obsession-exit --poweroff</command>
      </action>
    </item>
    <item label="_Redémarrer">
      <action name="Execute">
        <command>obsession-exit --reboot</command>
      </action>
    </item>
    <item label="_Hiberner">
      <action name="Execute">
        <command>obsession-exit --hibernate</command>
      </action>
    </item>
    <item label="_Suspendre">
      <action name="Execute">
        <command>obsession-exit --suspend</command>
      </action>
    </item>
    <!--
      <menu id="session" />
        <item label="Quitter">
        <action name="Execute">
          <command>lxsession-logout</command>
        </action>
      </item>
    -->
  </menu>
  <menu id="root-menu" label="Openbox 3">
    <separator label="Programmes" />
    %MENU%
    <separator label="Outils" />
    <menu id="info-menu" />
    <menu id="audacious" />
    <item label="_Firefox">
      <action name="Execute">
        <command>firefox</command>
      </action>
    </item>
    <item label="_Gestionnaire de fichiers">
      <action name="Execute">
        <command>pcmanfm</command>
      </action>
    </item>
    <menu id="bookmarks-menu" />
    <item label="_Terminal">
      <action name="Execute">
        <command>sakura</command>
      </action>
    </item>
    <menu id="openbox-DesktopSettings" />
    <separator label ="Bureau" />
    <menu id="openbox-menu" />
    <item label="_Verrouiller">
      <action name="Execute">
        <command>xlock -mode blank -dpmssuspend 10 -dpmsoff 45</command>
      </action>
    </item>
  </menu>
</openbox_menu>
