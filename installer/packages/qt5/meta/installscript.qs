function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType != "windows" && systemInfo.productType != "osx") component.addElevatedOperation("Execute", "chmod", "-R", "755", "@TargetDir@");
}
