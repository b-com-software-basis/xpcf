// See https://aka.ms/new-console-template for more information
using XPCF.Api;
using XPCF.Core;
using XPCF.Properties;
using XPCF.SampleComponent;
using reflect = System.Reflection;

Test t = new Test();
foreach (var type in typeof(IComponentManager).Assembly.GetTypes()) {
        Console.WriteLine(type.Name);
}
try
{
    IntrospectExtensions.xpcfComponentManager.load();
    IRegistryManager? registryMgr = IntrospectExtensions.xpcfComponentManager.getFactory().BindTo<IRegistryManager>();
    if (registryMgr is not null) {
    foreach (var interfaceMdata in registryMgr.getInterfacesMetadata()) {
        IntrospectExtensions.interfacesDict.TryAdd(interfaceMdata.name(), interfaceMdata.getUUID().ToString());
    }
    }
    bool result = t.loadModuleMetadata();
}
catch(System.TypeInitializationException e) {
    Console.WriteLine(e.Message);
    Console.WriteLine(e.StackTrace);
    if (e.InnerException is not null) {
        Console.WriteLine("InnerException: " + e.InnerException.Message);
    }
}
catch (System.Exception e)
{
    Console.WriteLine(e.Message);
    Console.WriteLine(e.StackTrace);
}

public static class Tester
{
    public static void testRequire(bool condition, string messageToThrow)
    {
        if (!condition)
        {
            throw new System.Exception(messageToThrow);
        }
    }
}
public static class IntrospectExtensions
{
    public static IComponentManager xpcfComponentManager = XPCF_Api.getComponentManagerInstance();
    public static Dictionary<string, string> interfacesDict = new Dictionary<string, string>();
    public static string GetUUID(string name)
    {// from aliasmanager?
        Console.WriteLine("GetUUID for " + name);
        string uuid = interfacesDict[name];
        return uuid;
    }

    public static UUID GetUUID<T>()
    {
        var name = typeof(T).Name;
        return new UUID(GetUUID(name));
    }

    [Obsolete("Use type parameter")]
    public static IComponentIntrospect Create<T>(this IComponentManager xpcfComponentManager)
    {
        return xpcfComponentManager.createComponent(GetUUID<T>());
    }

    public static IComponentIntrospect Create(this IComponentManager xpcfComponentManager, string type)
    {
        var uuid = new UUID(GetUUID(type));
        return xpcfComponentManager.createComponent(uuid);
    }

    public static IComponentIntrospect Create(this IComponentManager xpcfComponentManager, string type, string name)
    {
        var uuid = new UUID(GetUUID(type));
        return xpcfComponentManager.createComponent(name, uuid);
    }

    public static T? BindTo<T>(this IComponentIntrospect component) where T : class
    {
        return (T?)component.BindTo(typeof(T).Name);
    }

    public static object? BindTo(this IComponentIntrospect component, string name)
    {
        InterfaceMetadata? mData = null;
        foreach (UUID uuid in component.getInterfaces() ) {
            string compIFName = component.getMetadata(uuid).name();
            if (compIFName.Contains(name)) {
                mData = component.getMetadata(uuid);
                break;
            }
        }
        if (mData is null) {
            throw new System.Exception("Component doesn't implement interface " + name);
        }
        Type compType = component.GetType();
        reflect.MethodInfo? method = compType.GetMethod("bindTo_" + name);
        if (method is null) {
            throw new System.Exception("No method bindTo_"+ name + " available for the component: check swig binding !");
        }
        return method.Invoke(component, null);
    }
}

public class Test
{

    public void displayParameter(IProperty p)
    {
        Console.WriteLine("Property name =" + p.getName());
        for (uint i = 0; i < p.size(); i++)
        {
            switch (p.getType())
            {
                case IProperty.PropertyType.IProperty_NONE:
                    break;
                case IProperty.PropertyType.IProperty_INTEGER:
                    Console.WriteLine("=> Int property = " + p.getIntegerValue(i));
                    break;
                case IProperty.PropertyType.IProperty_UINTEGER:
                    Console.WriteLine("=> Uint property = " + p.getUnsignedIntegerValue(i));
                    break;
                case IProperty.PropertyType.IProperty_LONG:
                    Console.WriteLine("=> Long property = " + p.getLongValue(i));
                    break;
                case IProperty.PropertyType.IProperty_ULONG:
                    Console.WriteLine("=> ULong property = " + p.getUnsignedLongValue(i));
                    break;
                case IProperty.PropertyType.IProperty_CHARSTR:
                    Console.WriteLine("=> String property = " + p.getStringValue(i));
                    break;
                case IProperty.PropertyType.IProperty_UNICODESTR:
                    break;
                case IProperty.PropertyType.IProperty_FLOAT:
                    Console.WriteLine("=> Float property = " + p.getFloatingValue(i));
                    break;
                case IProperty.PropertyType.IProperty_DOUBLE:
                    Console.WriteLine("=> Double property = " + p.getDoubleValue(i));
                    break;
                case IProperty.PropertyType.IProperty_STRUCTURE:
                    {
                        IPropertyMap propertyMap = p.getStructureValue(i);
                        IEnumerator<IProperty> propertyEnum =
                                propertyMap.getProperties().getEnumerator();
                        Console.WriteLine("Accessing class values for structure from IProperty/IPropertyMap interfaces");
                        while (propertyEnum.MoveNext())
                        {
                            displayParameter(propertyEnum.Current);
                        }
                        break;
                    }
                default:
                    break;
            }
        }
    }
    public bool loadModuleMetadata()
    {
        IComponentManager xpcfComponentManager = IntrospectExtensions.xpcfComponentManager;
        xpcfComponentManager.load();
        XPCF.Core.UUID sampleComponentModuleUUID = new UUID("3b899ff0-e098-4218-bdaa-82abdec22960");
        XPCF.Core.UUID clsid_HumanMusician = new UUID("ae9d10bb-4bd1-462b-8c01-43f1815c6ae0");
        XPCF.Core.UUID iid_IHuman = new UUID("46333fd5-9eeb-4c9a-9912-b7df96ccf5fc");
        ModuleMetadata modMdata = xpcfComponentManager.findModuleMetadata(sampleComponentModuleUUID);
        string moduleName = modMdata.name();
        string path = modMdata.getPath();
        xpcfComponentManager.clear();
        IModuleManager xpcfModuleManager = XPCF_Api.getModuleManagerInstance();
        string xpcfVersion = xpcfModuleManager.getXpcfVersion(moduleName, path);
        Console.WriteLine("Module " + moduleName + " was built with xpcf version " + xpcfVersion);
        Tester.testRequire(XPCFErrorCode._SUCCESS == xpcfComponentManager.getFactory().BindTo<IRegistryManager>().loadModuleMetadata(moduleName, path), " failed to load module from moduleMetadata");
        InterfaceMetadata pI = xpcfComponentManager.findInterfaceMetadata(iid_IHuman);
        Tester.testRequire((pI is not null), "ERROR Cannot find interface iid_IHuman !");
        ComponentMetadata cmpMetadata = xpcfComponentManager.findComponentMetadata(clsid_HumanMusician);
        Tester.testRequire(cmpMetadata is not null, "ERROR Cannot get ComponentMetadata for component ID " + clsid_HumanMusician + " !");
        Console.WriteLine("Component Class clsid_HumanMusician found! Its Description is:" + cmpMetadata.description());
        string confPath = "xpcf_conf_test.xml";
        xpcfComponentManager.load(confPath);
        var vgComponent = xpcfComponentManager.createComponent("{63FF193D-93E6-4EDE-9947-22F864AC843F}");
        IConfigurable rIConfigurable = vgComponent.BindTo<IConfigurable>();
        IGuitarist? guitarist = vgComponent.BindTo<IGuitarist>();
        if (guitarist is not null) {
            guitarist.learn();
        }
        Console.WriteLine("Accessing class values for VirtualGuitarist from IProperty/IPropertyMap interfaces");
        foreach (var property in rIConfigurable.getProperties())
        {
            displayParameter(property);
        }
        /* catch (XPCF.Api.Exception e) { NOTE : exception not extended to derive from System.Exception... 
             Console.WriteLine("Catched xpcf exception : " + e.what());
         }*/
        xpcfComponentManager.clear();
        return true;
    }



}
