pragma Singleton
import QtQuick
import Bee as Bee

Bee.QmlUtils {
    id: utils
    
    // Additional QML-specific utility functions
    function formatValue(value, propertyInfo) {
        if (value === null) {
            return "null"
        } else if (value === undefined) {
            return "undefined"
        } else if (value === "") {
            return "\"\""
        } else if (propertyInfo && propertyInfo.isEnumType) {
            return enumValueName(propertyInfo.object, propertyInfo.name, value)
        } else {
            return "" + value
        }
    }
    
    // Get available interfaces for the interface selector
    function getAvailableInterfaces() {
        var interfaces = []
        
        // Add registered QML interfaces
        var registeredNames = Bee.Services.qmlInterface.registeredObjectsNames
        for (var i = 0; i < registeredNames.length; i++) {
            var name = registeredNames[i]
            if (Bee[name]) {
                interfaces.push({ name: name, interface: Bee[name] })
            }
        }
        
        return interfaces
    }
    
    // Check if a property value is a pointer to another object
    function isPointerProperty(propertyInfo) {
        return propertyInfo && propertyInfo.typeName && propertyInfo.typeName.indexOf("*") !== -1
    }

    // Check if a property is a map type (QMap, QVariantMap, etc.)
    function isMapPropertyType(propertyInfo) {
        return isMapProperty(propertyInfo)
    }

    // Get the object referenced by a pointer property
    function getPointerObject(parentObject, propertyName) {
        if (parentObject && propertyName) {
            return parentObject[propertyName]
        }
        return null
    }
}