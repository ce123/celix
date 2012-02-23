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
 * service_factory.h
 *
 *  Created on: Jun 26, 2011
 *      Author: alexander
 */

#ifndef SERVICE_FACTORY_H_
#define SERVICE_FACTORY_H_

#include <apr_general.h>

typedef struct service_factory * service_factory_t;

#include "celix_errno.h"
#include "service_registration.h"
#include "bundle.h"

struct service_factory {
    void *factory;
    celix_status_t (*getService)(void *factory, BUNDLE bundle, SERVICE_REGISTRATION registration, void **service);
    celix_status_t (*ungetService)(void *factory, BUNDLE bundle, SERVICE_REGISTRATION registration);
};


#endif /* SERVICE_FACTORY_H_ */