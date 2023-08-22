package com.gbox3d.blecentral

import android.content.Context
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import com.gbox3d.blecentral.databinding.ListItemBinding
import java.nio.ByteBuffer
import java.nio.ByteOrder


data class BluetoothDeviceItem(
    val address: String,
    var name: String?,
    var rssi: Int
)

class BluetoothDeviceAdapter(
    private val context: Context,
    private val devices: MutableList<BluetoothDeviceItem>
) : ArrayAdapter<BluetoothDeviceItem>(context, R.layout.list_item, devices) {

    override fun getView(position: Int, convertView: View?, parent: ViewGroup): View {
        val binding = convertView?.let {
            ListItemBinding.bind(it)
        } ?: ListItemBinding.inflate(LayoutInflater.from(context), parent, false)

        val device = devices[position]
        binding.deviceName.text = device.name ?: "Unknown"
        binding.deviceAddress.text = device.address
        binding.deviceRssi.text = "${device.rssi} dBm"

        return binding.root
    }
}

data class ResponsePacketHeader(
    val checkCode: Int,
    val cmd: Byte,
    val parm1: Byte,
    val parm2: Byte,
    val parm3: Byte
)

fun parseResponseHeader(responseData: ByteArray): ResponsePacketHeader? {
    val buffer = ByteBuffer.wrap(responseData)
    buffer.order(ByteOrder.LITTLE_ENDIAN)

    val checkCode = buffer.int
    val cmd = buffer.get()
    val parm1 = buffer.get()
    val parm2 = buffer.get()
    val parm3 = buffer.get()

    if (checkCode != myBlueTooth.CHECK_CODE) {
        Log.e("MainActivity", "Invalid check code in response!")
        return null
    }

    // Log.d("MainActivity", "Command: $cmd, Parameters: $parm1, $parm2, $parm3")

    return ResponsePacketHeader(checkCode, cmd, parm1, parm2, parm3)
}



class myBlueTooth {
    companion object {
        const val SERVICE_UUID = "02cb14bf-42e0-41bd-9d6d-68db02057e42"
        const val CHARACTERISTIC_UUID = "fef6efb5-fbf8-4d0a-a69f-57049733a87a"
        const val CLIENT_CHARACTERISTIC_CONFIG_UUID = "00002902-0000-1000-8000-00805f9b34fb"
        const val CHECK_CODE = 230815
    }


}