function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType == "windows")
	{
        component.addOperation("CreateShortcut", "@TargetDir@/Plans Converter.exe", "@StartMenuDir@/Plans Converter.lnk", "workingDirectory=@TargetDir@");
			
        component.addOperation("CreateShortcut", "@TargetDir@/Plans Converter.exe", "@DesktopDir@/Plans Converter.lnk", "workingDirectory=@TargetDir@");
    }
	else if (systemInfo.productType != "osx")
	{
		component.addElevatedOperation("Execute", "chmod", "-R", "755", "@TargetDir@");
		
		component.addElevatedOperation("CreateDesktopEntry", 
									   "/usr/share/applications/PlansConverter.desktop", 
                                       'Type=Application\nTerminal=false\nExec="@TargetDir@/run.sh"\nName=Plans Converter\nIcon=@TargetDir@/icon.svg\nCategories=Utility;');
	}
}
