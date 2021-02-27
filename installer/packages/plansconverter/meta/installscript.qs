function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows")
	{
        component.addOperation("CreateShortcut", "@TargetDir@/PlansConverter.exe", "@StartMenuDir@/Plans Converter.lnk",
            "workingDirectory=@TargetDir@");
			
        component.addOperation("CreateShortcut", "@TargetDir@/PlansConverter.exe", "@DesktopDir@/Plans Converter.lnk",
            "workingDirectory=@TargetDir@");
    }
	else
	{
		component.addElevatedOperation("Execute", "chmod", "-R", "755", "@TargetDir@");
		
		component.addElevatedOperation("CreateDesktopEntry", 
                                  "/usr/share/applications/PlansConverter.desktop", 
                                  'Type=Application\nTerminal=false\nExec="@TargetDir@/run.sh"\nName=Plans Converter\nIcon=@TargetDir@/icon.svg\nCategories=Utility;');
	}
}
