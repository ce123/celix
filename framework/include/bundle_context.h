/**
 *Licensed to the Apache Software Foundation (ASF) under one
 *or more contributor license agreements.  See the NOTICE file
 *distributed with this work for additional information
 *regarding copyright ownership.  The ASF licenses this file
 *to you under the Apache License, Version 2.0 (the
 *"License"); you may not use this file except in compliance
 *with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing,
 *software distributed under the License is distributed on an
 *"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 *specific language governing permissions and limitations
 *under the License.
 */
/*
 * bundle_context.h
 *
 *  \date       Mar 26, 2010
 *  \author    	<a href="mailto:dev@celix.apache.org">Apache Celix Project Team</a>
 *  \copyright	Apache License, Version 2.0
 */

#ifndef BUNDLE_CONTEXT_H_
#define BUNDLE_CONTEXT_H_

/**
 * A bundle's execution context within the Framework. The context is used to
 * grant access to other methods so that this bundle can interact with the
 * Framework.
 */
typedef struct bundleContext *bundle_context_pt;
typedef struct bundleContext bundle_context_t;


#include "service_factory.h"
#include "service_listener.h"
#include "bundle_listener.h"
#include "framework_listener.h"
#include "properties.h"
#include "array_list.h"

#ifdef __cplusplus
extern "C" {
#endif

celix_status_t
bundleContext_create(framework_pt framework, framework_logger_pt, bundle_pt bundle, bundle_context_pt *bundle_context);

celix_status_t bundleContext_destroy(bundle_context_pt context);

FRAMEWORK_EXPORT celix_status_t bundleContext_getBundle(bundle_context_pt context, bundle_pt *bundle);

FRAMEWORK_EXPORT celix_status_t bundleContext_getFramework(bundle_context_pt context, framework_pt *framework);

FRAMEWORK_EXPORT celix_status_t
bundleContext_installBundle(bundle_context_pt context, const char *location, bundle_pt *bundle);

FRAMEWORK_EXPORT celix_status_t
bundleContext_installBundle2(bundle_context_pt context, const char *location, const char *inputFile, bundle_pt *bundle);

FRAMEWORK_EXPORT celix_status_t
bundleContext_registerService(bundle_context_pt context, const char *serviceName, const void *svcObj,
                              properties_pt properties, service_registration_pt *service_registration);

FRAMEWORK_EXPORT celix_status_t
bundleContext_registerServiceFactory(bundle_context_pt context, const char *serviceName, service_factory_pt factory,
                                     properties_pt properties, service_registration_pt *service_registration);

/**
 * Register a C lang service to the framework.
 *
 * @param ctx The bundle context
 * @param serviceName the service name, cannot be NULL
 * @param svc the service object. Normally a pointer to a service struct (e.g. a struct with function pointers)
 * @param properties The meta properties assiated with the service. The service registration will take ownership of the properties
 * @return The serviceId or < 0 if registration was unsuccessful.
 */
long bundleContext_registerCService(bundle_context_t *ctx, const char *serviceName, const void *svc, properties_t *properties);

/**
* Register a service for the specified language to the framework.
*
* @param ctx The bundle context
* @param serviceName the service name, cannot be NULL
* @param svc the service object. Normally a pointer to a service struct (e.g. a struct with function pointers)
* @param properties The meta properties assiated with the service. The service registration will take ownership of the properties
* @return The serviceId or < 0 if registration was unsuccessful.
*/
long bundleContext_registerServiceForLang(bundle_context_t *ctx, const char *serviceName, const void *svc, properties_t *properties, const char* lang);

/**
 * Unregister the service with service id. The service will only be unregistered if the bundle of the bundle context
 * is the owner of the service.
 * TODO explain that this should nu be used in combination with the service_registration_t type.
 *
 * @param ctx The bundle context
 * @param serviceId The service id
 */
void bundleContext_unregisterService(bundle_context_t *ctx, long serviceId);


/**
 * Get and lock the service with the provided service id
 * Invokes the provided callback with the found service.
 * The svc, props and owner in the callback are only valid during the callback.
 * If no service is found the callback will not be invoked
 *
 * This function will block till the callback is finished. As result it is possible to provide callback data from the
 * stack.
 *
 * @param ctx The bundle context
 * @param serviceId the service id.
 * @param callbackHandle The data pointer, which will be used in the callbacks
 * @param use The callback, which will be called when service is retrieved.
 * @param bool returns true if a service was found.
 */
bool bundleContext_useServiceWithId(
        bundle_context_t *ctx,
        long serviceId,
        void *callbackHandle,
        void (*use)(void *handle, void* svc, const properties_t *props, const bundle_t *owner)
);

/**
 * Get a service reference for the bundle context. When the service reference is no longer needed use bundleContext_ungetServiceReference.
 * ServiceReference are coupled to a bundle context. Do not share service reference between bundles. Exchange the service.id instead.
 * 
 * @param context The bundle context
 * @param serviceName The name of the service (objectClass) to get
 * @param service_reference _output_ The found service reference, or NULL when no service is found.
 * @return CELIX_SUCCESS on success
 */
FRAMEWORK_EXPORT celix_status_t bundleContext_getServiceReference(bundle_context_pt context, const char *serviceName,
                                                                  service_reference_pt *service_reference);

/** Same as bundleContext_getServiceReference, but than for a optional serviceName combined with a optional filter.
 * The resulting array_list should be destroyed by the caller. For all service references return a unget should be called.
 * 
 * @param context the bundle context
 * @param serviceName the serviceName, can be NULL
 * @param filter the filter, can be NULL. If present will be combined (and) with the serviceName 
 * @param service_references _output_ a array list, can be size 0. 
 * @return CELIX_SUCCESS on success
 */
FRAMEWORK_EXPORT celix_status_t
bundleContext_getServiceReferences(bundle_context_pt context, const char *serviceName, const char *filter,
                                   array_list_pt *service_references);

/**
 * Retains (increases the ref count) the provided service reference. Can be used to retain a service reference.
 * Note that this is a deviation from the OSGi spec, due to the fact that C has no garbage collect.
 * 
 * @param context the bundle context
 * @param reference the service reference to retain
 * @return CELIX_SUCCES on success
 */
FRAMEWORK_EXPORT celix_status_t
bundleContext_retainServiceReference(bundle_context_pt context, service_reference_pt reference);

/**
 * Ungets the service references. If the ref counter of the service refernce goes to 0, the reference will be destroyed.
 * This is coupled with the bundleContext_getServiceReference(s) and bundleContext_retainServiceReferenc.
 * Note: That this is a deviation from the OSGi standard, due to the fact that C has no garbage collect.
 * 
 * @param context The bundle context.
 * @param reference the service reference to unget
 * @return CELIX_SUCCESS on success.
 */
FRAMEWORK_EXPORT celix_status_t
bundleContext_ungetServiceReference(bundle_context_pt context, service_reference_pt reference);

FRAMEWORK_EXPORT celix_status_t
bundleContext_getService(bundle_context_pt context, service_reference_pt reference, void **service_instance);

FRAMEWORK_EXPORT celix_status_t
bundleContext_ungetService(bundle_context_pt context, service_reference_pt reference, bool *result);

FRAMEWORK_EXPORT celix_status_t bundleContext_getBundles(bundle_context_pt context, array_list_pt *bundles);

FRAMEWORK_EXPORT celix_status_t bundleContext_getBundleById(bundle_context_pt context, long id, bundle_pt *bundle);

FRAMEWORK_EXPORT celix_status_t
bundleContext_addServiceListener(bundle_context_pt context, service_listener_pt listener, const char *filter);

FRAMEWORK_EXPORT celix_status_t
bundleContext_removeServiceListener(bundle_context_pt context, service_listener_pt listener);

FRAMEWORK_EXPORT celix_status_t bundleContext_addBundleListener(bundle_context_pt context, bundle_listener_pt listener);

FRAMEWORK_EXPORT celix_status_t
bundleContext_removeBundleListener(bundle_context_pt context, bundle_listener_pt listener);

FRAMEWORK_EXPORT celix_status_t
bundleContext_addFrameworkListener(bundle_context_pt context, framework_listener_pt listener);

FRAMEWORK_EXPORT celix_status_t
bundleContext_removeFrameworkListener(bundle_context_pt context, framework_listener_pt listener);

/**
 * Gets the config property - or environment variable if the config property does not exist - for the provided name.
 *
 * @param context The bundle context.
 * @param name The name of the config property / environment variable to get.
 * @param value A ptr to the output value. This will be set when a value is found or else will be set to NULL.
 * @return 0 if successful.
 */
FRAMEWORK_EXPORT celix_status_t
bundleContext_getProperty(bundle_context_pt context, const char *name, const char **value);

/**
 * Gets the config property - or environment variable if the config property does not exist - for the provided name.
 *
 * @param context The bundle context.
 * @param name The name of the config property / environment variable to get.
 * @param defaultValue The default value to return if no value is found.
 * @param value A ptr to the output value. This will be set when a value is found or else will be set to NULL.
 * @return 0 if successful.
 */
FRAMEWORK_EXPORT celix_status_t
bundleContext_getPropertyWithDefault(bundle_context_pt context, const char *name, const char *defaultValue, const char **value);

#ifdef __cplusplus
}
#endif

#endif /* BUNDLE_CONTEXT_H_ */
