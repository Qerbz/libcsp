/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 Gomspace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <FreeRTOS.h>
#include <semphr.h>

#include <csp/arch/csp_semaphore.h>
#include <csp/csp_debug.h>
#include <csp/csp.h>

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
int csp_mutex_create(csp_mutex_t * mutex) {
	*mutex = xSemaphoreCreateMutex();
	if (*mutex) {
		return CSP_SEMAPHORE_OK;
	} else {
		return CSP_SEMAPHORE_ERROR;
	}
}

int csp_mutex_remove(csp_mutex_t * mutex) {
	return csp_bin_sem_remove(mutex);
}
#endif

void csp_mutex_create_static(csp_mutex_t * handle, csp_mutex_buffer_t * buffer) {
	*handle = xSemaphoreCreateMutexStatic(buffer);
}

int csp_mutex_lock(csp_mutex_t * mutex, uint32_t timeout) {
	return csp_bin_sem_wait(mutex, timeout);
}

int csp_mutex_unlock(csp_mutex_t * mutex) {
	return csp_bin_sem_post(mutex);
}

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
int csp_bin_sem_create(csp_bin_sem_handle_t * sem) {
	vSemaphoreCreateBinary(*sem);
	return CSP_SEMAPHORE_OK;
}

int csp_bin_sem_remove(csp_bin_sem_handle_t * sem) {
	if ((sem != NULL) && (*sem != NULL)) {
		/* It is safe to call on static allocated types,
		   freertos remembers which type of allocation was used */
		vSemaphoreDelete(*sem);
	}
	return CSP_SEMAPHORE_OK;
}
#endif

void csp_bin_sem_create_static(csp_bin_sem_handle_t * handle, csp_bin_sem_t * buffer) {
	*handle = xSemaphoreCreateBinaryStatic(buffer);
	xSemaphoreGive(*handle);
}

int csp_bin_sem_wait(csp_bin_sem_handle_t * sem, uint32_t timeout) {
	csp_log_lock("Wait: %p", sem);
	if (timeout != CSP_MAX_TIMEOUT) {
		timeout = timeout / portTICK_PERIOD_MS;
	}
	if (xSemaphoreTake(*sem, timeout) == pdPASS) {
		return CSP_SEMAPHORE_OK;
	}
	return CSP_SEMAPHORE_ERROR;
}

int csp_bin_sem_post(csp_bin_sem_handle_t * sem) {
	csp_log_lock("Post: %p", sem);
	if (xSemaphoreGive(*sem) == pdPASS) {
		return CSP_SEMAPHORE_OK;
	}
	return CSP_SEMAPHORE_ERROR;
}

int csp_bin_sem_post_isr(csp_bin_sem_handle_t * sem, int * pxTaskWoken) {
	csp_log_lock("Post: %p", sem);
	if (xSemaphoreGiveFromISR(*sem, (void *)pxTaskWoken) == pdPASS) {
		return CSP_SEMAPHORE_OK;
	} else {
		return CSP_SEMAPHORE_ERROR;
	}
}
