// See https://aka.ms/new-console-template for more information
using XPCF.Api;
using XPCF.Core;
using XPCF.Collection;
using XPCF.Properties;
using XPCF.SampleComponent;
using XPCF.Bindings;
using reflect = System.Reflection;

IntrospectExtensions.xpcfComponentManager = XPCF_Api.getComponentManagerInstance();

Test t = new Test();
try
{
    IntrospectExtensions.xpcfComponentManager.load();
    IntrospectExtensions.populateRegistry();
    bool result = t.loadModuleMetadata();
}
catch (System.TypeInitializationException e)
{
    Console.WriteLine(e.Message);
    Console.WriteLine(e.StackTrace);
    if (e.InnerException is not null)
    {
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
    public static Dictionary<string, string> componentsDict = new Dictionary<string, string>();

    public static void populateRegistry() {
        IRegistryManager? registryMgr = IntrospectExtensions.xpcfComponentManager.getFactory().BindTo<IRegistryManager>();
        if (registryMgr is not null)
        {
            foreach (var interfaceMdata in registryMgr.getInterfacesMetadata())
            {
                IntrospectExtensions.interfacesDict.TryAdd(interfaceMdata.name(), interfaceMdata.getUUID().ToString());
            }
            foreach (var moduleMData in registryMgr.getModulesMetadata())
            {
                IntrospectExtensions.modulesDict.TryAdd(moduleMData.name(), moduleMData.getUUID().ToString());
                foreach (var componentMData in moduleMData.getComponentsMetadata())
                {
                    IntrospectExtensions.componentsDict.TryAdd(componentMData.name(), componentMData.getUUID().ToString());
                }
            }
        }
    }
    public static Dictionary<string, string> modulesDict = new Dictionary<string, string>();
    public static string GetUUID(string name)
    {
        Console.WriteLine("GetUUID for " + name);
        if (interfacesDict.ContainsKey(name))
            return interfacesDict[name];
        if (componentsDict.ContainsKey(name))
            return componentsDict[name];
        if (modulesDict.ContainsKey(name))
            return modulesDict[name];
        return "";
    }

    public static string GetUUID(IAliasManager mgr, string name)
    {
        Console.WriteLine("GetUUID from AliasManager for " + name);
        if (mgr.aliasExists(IAliasManager.Type.Component, name)) {
            return mgr.resolveComponentAlias(name).ToString();
        }
        if (mgr.aliasExists(IAliasManager.Type.Interface, name)) {
            return mgr.resolveInterfaceAlias(name).ToString();
        }
        if (mgr.aliasExists(IAliasManager.Type.Module, name)) {
            return mgr.resolveModuleAlias(name).ToString();
        }
        return "";
    }

    public static T handleNull<T>(T? obj)
    {
        if (obj is null)
        {
            throw new System.Exception("Object is null " + typeof(T).Name);
        }
        return obj!;
    }

    public static T Resolve<T>(this IComponentManager cmpMgr) where T : class
    {
        return cmpMgr.getFactory().Resolve<T>();
    }

    public static T Resolve<T>(this IFactory factory) where T : class
    {
        return (T)Resolve(factory, typeof(T).Name);
    }

    public static object Resolve(this IFactory factory, string name)
    {
        Type compType = typeof(XPCF_Bindings);
        reflect.MethodInfo? method = compType.GetMethod("resolve_" + name, reflect.BindingFlags.Public | reflect.BindingFlags.Static, null, new System.Type[] { typeof(IFactory) }, null);
        if (method is null)
        {
            throw new System.Exception("No method resolve_" + name + " available : check swig binding !");
        }
        return method.Invoke(null, new object[] { factory })!;
    }

    public static T Resolve<T>(this IComponentManager cmpMgr, string instanceName) where T : class
    {
        return cmpMgr.getFactory().Resolve<T>(instanceName);
    }

    public static T Resolve<T>(this IFactory factory, string instanceName) where T : class
    {
        return (T)Resolve(factory, typeof(T).Name, instanceName);
    }

    public static object Resolve(this IFactory factory, string name, string instanceName)
    {
        Type compType = typeof(XPCF_Bindings);
        reflect.MethodInfo? method = compType.GetMethod("resolve_" + name, reflect.BindingFlags.Public | reflect.BindingFlags.Static, null, new System.Type[] { typeof(IFactory), typeof(string) }, null);
        if (method is null)
        {
            throw new System.Exception("No method resolve_" + name + " available : check swig binding !");
        }
        return method.Invoke(null, new object[] { factory, instanceName })!;
    }

    public static IComponentIntrospectEnumerable ResolveAll<T>(this IComponentManager cmpMgr) where T : class
    {
        return cmpMgr.getFactory().ResolveAll<T>();
    }

    public static IComponentIntrospectEnumerable ResolveAll<T>(this IFactory factory) where T : class
    {
        return (IComponentIntrospectEnumerable)ResolveAll(factory, typeof(T).Name);
    }

    public static object ResolveAll(this IFactory component, string name)
    {
        Type compType = typeof(XPCF_Bindings);
        reflect.MethodInfo? method = compType.GetMethod("resolveAll_" + name, reflect.BindingFlags.Public | reflect.BindingFlags.Static);
        if (method is null)
        {
            throw new System.Exception("No method resolveAll_" + name + " available : check swig binding !");
        }
        return method.Invoke(null, new object[] { component })!;
    }

    public static T BindTo<T>(this IComponentIntrospect component) where T : class
    {
        return (T)component.BindTo(typeof(T).Name);
    }

    public static object BindTo(this IComponentIntrospect component, string name)
    {
        InterfaceMetadata? mData = null;
        foreach (UUID uuid in component.getInterfaces())
        {
            string compIFName = component.getMetadata(uuid).name();
            if (compIFName.Contains(name))
            {
                mData = component.getMetadata(uuid);
                break;
            }
        }
        if (mData is null)
        {
            throw new System.Exception("Component doesn't implement interface " + name);
        }
        Type compType = typeof(XPCF_Bindings);
        reflect.MethodInfo? method = compType.GetMethod("bindTo_" + name, reflect.BindingFlags.Public | reflect.BindingFlags.Static);
        if (method is null)
        {
            throw new System.Exception("No method bindTo_" + name + " available for the component: check swig binding !");
        }
        return method.Invoke(null, new object[] { component })!;
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
        IGuitarist guitarist = vgComponent.BindTo<IGuitarist>();
        guitarist.learn();
        Console.WriteLine("Resolving component for IElectricGuitar interface");
        IElectricGuitar elecGuitar = xpcfComponentManager.Resolve<IElectricGuitar>();
        IGuitar guitar = elecGuitar.BindTo<IGuitar>();
        Console.WriteLine("Electric guitar brand is " + guitar.getGuitarBrand());
        Console.WriteLine("Electric guitar string number is " + guitar.getNbStrings());
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
