/* mbed Microcontroller Library
 * Copyright (c) 2017-2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BLE_PAL_ATTCLIENTTOGATTCLIENTADAPTER_H_
#define BLE_PAL_ATTCLIENTTOGATTCLIENTADAPTER_H_

#include "AttClient.h"
#include "GattClient.h"

namespace ble {
namespace pal {

/**
 * Adapt a pal::AttClient into a pal::GattClient.
 *
 * This class let vendors define their abstraction layer in term of an AttClient
 * and adapt any AttClient into a GattClient.
 */
class AttClientToGattClientAdapter : public GattClient {
    static const uint16_t END_ATTRIBUTE_HANDLE = 0xFFFF;
    static const uint16_t SERVICE_TYPE_UUID = 0x2800;
    static const uint16_t INCLUDE_TYPE_UUID = 0x2802;
    static const uint16_t CHARACTERISTIC_TYPE_UUID = 0x2803;

    /**
     * Construct an instance of GattClient from an instance of AttClient.
     * @param client The client to adapt.
     */
    AttClientToGattClientAdapter(AttClient& client) :
        GattClient(), _client(client) {
        _client.when_server_message_received(
            mbed::callback(this, &AttClientToGattClientAdapter::on_server_event)
        );
        _client.when_transaction_timeout(
            mbed::callback(
                this, &AttClientToGattClientAdapter::on_transaction_timeout
            )
        );
    }

    /**
     * @see ble::pal::GattClient::exchange_mtu
     */
    virtual ble_error_t exchange_mtu(connection_handle_t connection) {
        return _client.exchange_mtu_request(connection);
    }

    /**
     * @see ble::pal::GattClient::get_mtu_size
     */
    virtual ble_error_t get_mtu_size(
        connection_handle_t connection_handle,
        uint16_t& mtu_size
    ) {
        return _client.get_mtu_size(connection_handle, mtu_size);
    }

    /**
     * @see ble::pal::GattClient::discover_primary_service
     */
    virtual ble_error_t discover_primary_service(
        connection_handle_t connection,
        attribute_handle_t discovery_range_begining
    ) {
        return _client.read_by_group_type_request(
            connection,
            attribute_handle_range(discovery_range_begining, END_ATTRIBUTE_HANDLE),
            SERVICE_TYPE_UUID
        );
    }

    /**
     * @see ble::pal::GattClient::discover_primary_service_by_service_uuid
     */
    virtual ble_error_t discover_primary_service_by_service_uuid(
        connection_handle_t connection_handle,
        attribute_handle_t discovery_range_begining,
        const UUID& uuid
    ) {
        return _client.find_by_type_value_request(
            connection_handle,
            attribute_handle_range(discovery_range_begining, END_ATTRIBUTE_HANDLE),
            SERVICE_TYPE_UUID,
            array_view<const uint8_t>(
                uuid.getBaseUUID(),
                (uuid.shortOrLong() == UUID::UUID_TYPE_SHORT) ? 2 : UUID::LENGTH_OF_LONG_UUID
            )
        );
    }

    /**
     * @see ble::pal::GattClient::find_included_service
     */
    virtual ble_error_t find_included_service(
        connection_handle_t connection_handle,
        attribute_handle_range_t service_range
    ) {
        return _client.read_by_type_request(
            connection_handle,
            service_range,
            INCLUDE_TYPE_UUID
        );
    }

    /**
     * @see ble::pal::GattClient::discover_characteristics_of_a_service
     */
    virtual ble_error_t discover_characteristics_of_a_service(
        connection_handle_t connection_handle,
        attribute_handle_range_t discovery_range
    ) {
        return _client.read_by_type_request(
            connection_handle,
            discovery_range,
            CHARACTERISTIC_TYPE_UUID
        );
    }

    /**
     * @see ble::pal::GattClient::discover_characteristics_descriptors
     */
    virtual ble_error_t discover_characteristics_descriptors(
        connection_handle_t connection_handle,
        attribute_handle_range_t descriptors_discovery_range
    ) {
        return _client.find_information_request(
            connection_handle,
            descriptors_discovery_range
        );
    }

    /**
     * @see ble::pal::GattClient::read_attribute_value
     */
    virtual ble_error_t read_attribute_value(
        connection_handle_t connection_handle,
        attribute_handle_t attribute_handle
    ) {
        return _client.read_request(
            connection_handle,
            attribute_handle
        );
    }

    /**
     * @see ble::pal::GattClient::read_using_characteristic_uuid
     */
    virtual ble_error_t read_using_characteristic_uuid(
        connection_handle_t connection_handle,
        attribute_handle_range_t read_range,
        const UUID& uuid
    ) {
        return _client.read_by_type_request(
            connection_handle,
            read_range,
            uuid
        );
    }

    /**
     * @see ble::pal::GattClient::read_attribute_blob
     */
    virtual ble_error_t read_attribute_blob(
        connection_handle_t connection_handle,
        attribute_handle_t attribute_handle,
        uint16_t offset
    ) {
        return _client.read_blob_request(
            connection_handle,
            attribute_handle,
            offset
        );
    }

    /**
     * @see ble::pal::GattClient::read_multiple_characteristic_values
     */
    virtual ble_error_t read_multiple_characteristic_values(
        connection_handle_t connection_handle,
        const array_view<const attribute_handle_t>& characteristic_value_handles
    ) {
        return _client.read_multiple_request(
            connection_handle,
            characteristic_value_handles
        );
    }

    /**
     * @see ble::pal::GattClient::write_without_response
     */
    virtual ble_error_t write_without_response(
        connection_handle_t connection_handle,
        attribute_handle_t characteristic_value_handle,
        const array_view<const uint8_t>& value
    ) {
        return _client.write_command(
            connection_handle,
            characteristic_value_handle,
            value
        );
    }

    /**
     * @see ble::pal::GattClient::signed_write_without_response
     */
    virtual ble_error_t signed_write_without_response(
        connection_handle_t connection_handle,
        attribute_handle_t characteristic_value_handle,
        const array_view<const uint8_t>& value
    ) {
        return _client.signed_write_command(
            connection_handle,
            characteristic_value_handle,
            value
        );
    }

    /**
     * @see ble::pal::GattClient::write_attribute
     */
    virtual ble_error_t write_attribute(
        connection_handle_t connection_handle,
        attribute_handle_t attribute_handle,
        const array_view<const uint8_t>& value
    ) {
        return _client.write_request(
            connection_handle,
            attribute_handle,
            value
        );
    }

    /**
     * @see ble::pal::GattClient::queue_prepare_write
     */
    virtual ble_error_t queue_prepare_write(
        connection_handle_t connection_handle,
        attribute_handle_t characteristic_value_handle,
        const array_view<const uint8_t>& value,
        uint16_t offset
    ) {
        return _client.prepare_write_request(
            connection_handle,
            characteristic_value_handle,
            offset,
            value
        );
    }

    /**
     * @see ble::pal::GattClient::execute_write_queue
     */
    virtual ble_error_t execute_write_queue(
        connection_handle_t connection_handle,
        bool execute
    ) {
        return _client.execute_write_request(connection_handle, execute);
    }

    /**
     * @see ble::pal::GattClient::initialize
     */
    virtual ble_error_t initialize() {
        return _client.initialize();
    }

    /**
     * @see ble::pal::GattClient::terminate
     */
    virtual ble_error_t terminate() {
        return _client.initialize();
    }

private:
    AttClient& _client;
};

} // namespace pal
} // namespace ble


#endif /* BLE_PAL_ATTCLIENTTOGATTCLIENTADAPTER_H_ */