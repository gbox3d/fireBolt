package com.gbox3d.blecentral

import android.Manifest
import android.annotation.SuppressLint
import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.bluetooth.BluetoothGattDescriptor
import android.bluetooth.BluetoothManager
import android.bluetooth.le.BluetoothLeScanner
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanResult
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import android.widget.TextView
import androidx.activity.result.contract.ActivityResultContracts
import androidx.annotation.RequiresApi
import androidx.core.app.ActivityCompat
import com.gbox3d.blecentral.databinding.ActivityMainBinding
import com.gbox3d.blecentral.databinding.ListItemBinding
import com.gbox3d.blecentral.myBlueTooth.Companion.CHARACTERISTIC_UUID
import com.gbox3d.blecentral.myBlueTooth.Companion.CHECK_CODE
import com.gbox3d.blecentral.myBlueTooth.Companion.CLIENT_CHARACTERISTIC_CONFIG_UUID
import com.gbox3d.blecentral.myBlueTooth.Companion.SERVICE_UUID
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.util.UUID


class MainActivity : AppCompatActivity() {

    companion object {
        private const val REQUEST_LOCATION_PERMISSION = 1
        private const val REQUEST_BLUETOOTH_SCAN_PERMISSION = 2 // 권한 요청 코드 정의
        private const val REQUEST_BLUETOOTH_CONNECT = 3
    }



    private lateinit var bluetoothAdapter: BluetoothAdapter
    private lateinit var bluetoothLeScanner: BluetoothLeScanner

    private lateinit var binding: ActivityMainBinding
    private val devices = mutableListOf<BluetoothDeviceItem>()
    private val adapter by lazy { BluetoothDeviceAdapter(this, devices) }

    private var scanning = false
    private var currentSelectedDevice: BluetoothDeviceItem? = null

    private fun checkPermissions() {

        //BLUETOOTH_SCAN  은 API 레벨 31 부터 이므로 그이하는 ACCESS_FINE_LOCATION 로 대신한다.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.BLUETOOTH_SCAN), REQUEST_BLUETOOTH_SCAN_PERMISSION)
            }
            else {
                Log.d("MainActivity","already scan permission ok")
                binding.tvInfoMsg.setText("already BLUETOOTH_SCAN permission ok")
            }

            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.BLUETOOTH_CONNECT), REQUEST_BLUETOOTH_CONNECT)
            } else {
                // 권한이 이미 있는 경우 연결 코드 실행
                Log.d("MainActivity","already connect permission ok")
                binding.tvInfoMsg.text = binding.tvInfoMsg.text.toString() + "\n already connect permission ok"
            }
        }
        else {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.ACCESS_FINE_LOCATION), REQUEST_LOCATION_PERMISSION)
            }
            else {
                Log.d("MainActivity","already location permission ok")
                binding.tvInfoMsg.setText("already location permission ok")
            }
        }




    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        when (requestCode) {
            REQUEST_LOCATION_PERMISSION -> {
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // 권한이 부여되었습니다. 스캔을 시작하거나 다른 작업을 수행할 수 있습니다.
                    Log.d("MainActivity","location permission ok")
                } else {
                    // 권한이 거부되었습니다. 사용자에게 필요한 이유를 설명하거나 다른 조치를 취할 수 있습니다.
                    Log.d("MainActivity","location permission deney")
                    
                }
            }
            REQUEST_BLUETOOTH_SCAN_PERMISSION -> {
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // 권한이 부여되었습니다. 스캔을 시작하거나 다른 작업을 수행할 수 있습니다.
                    Log.d("MainActivity","scan permission ok")
                } else {
                    // 권한이 거부되었습니다. 사용자에게 필요한 이유를 설명하거나 다른 조치를 취할 수 있습니다.
                    Log.d("MainActivity","scan permission deney")
                }
            }
            REQUEST_BLUETOOTH_CONNECT -> {
            if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // 권한이 승인된 경우 연결 코드 실행
                    Log.d("MainActivity","connect permission ok")
                } else {
                    // 권한이 거부된 경우 사용자에게 알림 표시
                    Log.d("MainActivity","connect permission deney")
                }
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        //setContentView(R.layout.activity_main)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        binding.listDevices.adapter = adapter

        checkPermissions()

        val bluetoothManager = getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        bluetoothAdapter = bluetoothManager.adapter
        bluetoothLeScanner = bluetoothAdapter.bluetoothLeScanner

        //scan button
        binding.buttonScan.setOnClickListener {
            if (bluetoothAdapter == null || !bluetoothAdapter.isEnabled) {

                // 블루투스가 비활성화되어 있으므로 사용자에게 블루투스 활성화를 요청
                
                val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
                bluetoothActivityResultLauncher.launch(enableBtIntent)

                Log.d("MainActivity","bluetooth enable request")
                
            } else {
                if(!scanning) {
                    // 블루투스가 이미 활성화되어 있으므로 스캔 시작
                    Log.d("MainActivity", "bluetooth scan start")


                    var _permision:String = ""
                    var _requestCode:Int = REQUEST_BLUETOOTH_SCAN_PERMISSION

                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                        _permision = Manifest.permission.BLUETOOTH_SCAN
                        Log.d("MainActivity", "req permission BLUETOOTH_SCAN")
                        _requestCode = REQUEST_BLUETOOTH_SCAN_PERMISSION
                        
                    }
                    else {
                        _permision = Manifest.permission.ACCESS_FINE_LOCATION
                        Log.d("MainActivity", "req permission ACCESS_FINE_LOCATION")
                        _requestCode = REQUEST_LOCATION_PERMISSION
                    }

                    if (ActivityCompat.checkSelfPermission(
                            this,
                            _permision
                        ) != PackageManager.PERMISSION_GRANTED
                    ) {
                        
                        ActivityCompat.requestPermissions(
                            this,
                            arrayOf(_permision),
                            _requestCode
                        )
                    }
                    else {
                        bluetoothLeScanner.startScan(scanCallback)
                        scanning = true
                        binding.buttonScan.setText("Stop scan")
                    }
                }
                else {
                    Log.d("MainActivity", "bluetooth scan stop")
//                    stopScanning()
                    bluetoothLeScanner.stopScan(scanCallback)
                    scanning = false
                    binding.buttonScan.setText("Start scan")
                }
            }
        }

        fun _sendReqRead(gatt: BluetoothGatt) {
            // 0x06 명령어를 사용하여 설정 파일 읽기 요청
            val commandPacket = ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN)
            commandPacket.putInt(CHECK_CODE)
            commandPacket.put(0x06.toByte()) // cmd
            commandPacket.put(byteArrayOf(0, 0, 0)) // parm


            var _permision:String = ""
            var _requestCode:Int = REQUEST_BLUETOOTH_CONNECT

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                _permision = Manifest.permission.BLUETOOTH_CONNECT
                _requestCode = REQUEST_BLUETOOTH_CONNECT
                Log.d("MainActivity", "req permission BLUETOOTH_SCAN")

            }
            else {
                _permision = Manifest.permission.ACCESS_FINE_LOCATION
                _requestCode = REQUEST_LOCATION_PERMISSION
                Log.d("MainActivity", "req permission ACCESS_FINE_LOCATION")
            }

            val characteristic = gatt?.getService(UUID.fromString(SERVICE_UUID))
                ?.getCharacteristic(UUID.fromString(CHARACTERISTIC_UUID))


//                        characteristic?.setValue(commandPacket.array())
//                        gatt?.writeCharacteristic(characteristic)
            if (ActivityCompat.checkSelfPermission(
                    this@MainActivity,
                    _permision
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                ActivityCompat.requestPermissions(
                    this@MainActivity,
                    arrayOf(_permision),
                    _requestCode
                )
                Log.d("MainActivity", "request permission $_permision" )
                return
            }

            Log.d("MainActivity", "write data : ${commandPacket.array()}")

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {

                try {
                    val writeCharacteristicMethod = gatt?.javaClass?.getMethod(
                        "writeCharacteristic",
                        BluetoothGattCharacteristic::class.java,
                        ByteArray::class.java,
                        Int::class.javaPrimitiveType
                    )
                    writeCharacteristicMethod?.invoke(
                        gatt,
                        characteristic,
                        commandPacket.array(),
                        BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
                    )
                } catch (e: Exception) {
                    // 메서드 호출에 실패한 경우, 예외 처리
                }
            }
            else {
                characteristic?.setValue(commandPacket.array())
                gatt?.writeCharacteristic(characteristic)
            }

        }

        var gatt:BluetoothGatt? = null
//        var characteristic:BluetoothGattCharacteristic? = null

        binding.buttonConect.setOnClickListener {

            //선택된 디바이스가 없으면 경고메쎄지 출력
            if(currentSelectedDevice == null) {
                Log.d("MainActivity", "no selected device")
                binding.tvInfoMsg.setText("no selected device")
                
            }
            else {
                //연결을 시도한다.
                Log.d("MainActivity", "try connect to ${currentSelectedDevice!!.address}")
                binding.tvInfoMsg.setText("connect to ${currentSelectedDevice!!.address}")

                // 연결을 시도하는 코드
                val device = bluetoothAdapter.getRemoteDevice(currentSelectedDevice!!.address)

                gatt = device.connectGatt(this, false, object : BluetoothGattCallback() {

                    override fun onDescriptorWrite(gatt: BluetoothGatt?, descriptor: BluetoothGattDescriptor?, status: Int) {
                        if (status == BluetoothGatt.GATT_SUCCESS) {
                            Log.d("MainActivity", "Descriptor write successful")
                        } else {
                            Log.e("MainActivity", "Descriptor write failed: $status")
                        }
                    }


                    override fun onCharacteristicChanged(gatt: BluetoothGatt?, characteristic: BluetoothGattCharacteristic?) {
                        val data = characteristic?.value // 변경된 데이터
                        Log.d("MainActivity","onCharacteristicChanged")
                        // 데이터 처리 로직
                        if (data != null) {
                            val dataLength = data.size
                            Log.d("MainActivity", "Changed data length: $dataLength")
                            Log.d("MainActivity", "Changed data: ${data.joinToString(", ")}")
                        }
                    }

                    override fun onMtuChanged(gatt: BluetoothGatt?, mtu: Int, status: Int) {
                        super.onMtuChanged(gatt, mtu, status)
                        if (status == BluetoothGatt.GATT_SUCCESS) {
                            Log.d("MainActivity", "MTU changed to $mtu")
                        } else {
                            Log.e("MainActivity", "MTU change failed")
                        }
                    }


                    override fun onServicesDiscovered(gatt: BluetoothGatt?, status: Int) {

                        Log.d("MainActivity","onServicesDiscovered")

                        if (status == BluetoothGatt.GATT_SUCCESS) {
                            Log.d("MainActivity", "onServicesDiscovered received: $status")

                            val characteristicToNotify = gatt?.getService(UUID.fromString(SERVICE_UUID))?.getCharacteristic(UUID.fromString(CHARACTERISTIC_UUID))
                            characteristicToNotify?.let {
                                if (ActivityCompat.checkSelfPermission(
                                        this@MainActivity,
                                        Manifest.permission.ACCESS_FINE_LOCATION
                                    ) != PackageManager.PERMISSION_GRANTED
                                ) {
                                    // TODO: Consider calling
                                    //    ActivityCompat#requestPermissions
                                    // here to request the missing permissions, and then overriding
                                    //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                                    //                                          int[] grantResults)
                                    // to handle the case where the user grants the permission. See the documentation
                                    // for ActivityCompat#requestPermissions for more details.
                                    return
                                }

                                Log.d("MainActivity", "setCharacteristicNotification")
                                
                                gatt?.setCharacteristicNotification(it, true)
                                val descriptor = it.getDescriptor(UUID.fromString(CLIENT_CHARACTERISTIC_CONFIG_UUID))
                                descriptor?.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
                                gatt?.writeDescriptor(descriptor)

                                // 72바이트로 MTU 크기 변경 요청
                                gatt?.requestMtu(72)
                            }

                            val characteristicToRead = gatt?.getService(UUID.fromString(SERVICE_UUID))?.getCharacteristic(UUID.fromString(CHARACTERISTIC_UUID))
                            characteristicToRead?.let {
                                gatt.readCharacteristic(it)
                            }

//                            gatt?.services?.forEach { service ->
//                                Log.d("MainActivity", "Found service: ${service.uuid}")
//
//                                // 해당 서비스의 특성도 출력
//                                service.characteristics.forEach { characteristic ->
//                                    Log.d("MainActivity", "  - Characteristic: ${characteristic.uuid}")
//                                }
//                            }
                        } else {
                            Log.w("MainActivity", "onServicesDiscovered received: $status")
                        }

                    }

                    override fun onCharacteristicRead(gatt: BluetoothGatt?, characteristic: BluetoothGattCharacteristic?, status: Int) {

                        Log.d("MainActivity","onCharacteristicRead")

                        val data = characteristic?.value
                        if (data != null) {

                            //데이터 길이
                            val dataLength = data.size
                            Log.d("MainActivity", "Read data length: $dataLength")
                            Log.d("MainActivity", "Read data: ${data.joinToString(", ")}")
                            val _header: ResponsePacketHeader? = parseResponseHeader(data)

                            Log.d("MainActivity","check code : ${_header?.checkCode} , cmd : ${_header?.cmd}")
                        }
                    }

                    override fun onConnectionStateChange(gatt: BluetoothGatt?, status: Int, newState: Int) {
                        when (newState) {
                            BluetoothGatt.STATE_CONNECTED -> {
                                Log.d("MainActivity", "Connected to GATT server.")

                                var _permision:String = ""
                                var _requestCode:Int = REQUEST_BLUETOOTH_CONNECT

                                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                                    _permision = Manifest.permission.BLUETOOTH_CONNECT
                                    _requestCode = REQUEST_BLUETOOTH_CONNECT
                                    Log.d("MainActivity", "req permission BLUETOOTH_SCAN")

                                }
                                else {
                                    _permision = Manifest.permission.ACCESS_FINE_LOCATION
                                    _requestCode = REQUEST_LOCATION_PERMISSION
                                    Log.d("MainActivity", "req permission ACCESS_FINE_LOCATION")
                                }

//                                //get characteristic
//                                val characteristic = gatt?.getService(UUID.fromString(SERVICE_UUID))
//                                    ?.getCharacteristic(UUID.fromString(CHARACTERISTIC_UUID))
//

                                if (ActivityCompat.checkSelfPermission(
                                        this@MainActivity,
                                        _permision
                                    ) != PackageManager.PERMISSION_GRANTED
                                ) {
                                    ActivityCompat.requestPermissions(
                                        this@MainActivity,
                                        arrayOf(_permision),
                                        _requestCode
                                    )
                                    Log.d("MainActivity", "request permission $_permision" )
                                    return
                                }


                                Log.d("MainActivity", "start discover service")

                                gatt?.discoverServices() // 서비스 발견 시작
                            }
                            BluetoothGatt.STATE_DISCONNECTED -> {
                                Log.d("MainActivity", "Disconnected from GATT server.")
                            }
                        }
                    }

                })



                
            }

        }

        binding.buttonRead.setOnClickListener {
            gatt?.let { it1 -> _sendReqRead(it1) }

        }

        binding.buttonWrite.setOnClickListener {

        }

        binding.buttonReadCharistic.setOnClickListener {

            gatt?.let {
                it1->
                    val characteristic = it1.getService(UUID.fromString(SERVICE_UUID))
                        .getCharacteristic(UUID.fromString(CHARACTERISTIC_UUID))
                    it1.readCharacteristic(characteristic)
            }

        }

        //list event
        binding.listDevices.setOnItemClickListener { parent, view, position, id ->
            val device = devices[position]
            Log.d("MainActivity", "Clicked device: ${device.address}")
            binding.tvInfoMsg.setText("Clicked device: ${device.address}")
            currentSelectedDevice = device
        }
    }


    private val scanCallback = object : ScanCallback() {

        override fun onScanResult(callbackType: Int, result: ScanResult) {

            val device = result.device
            val rssi = result.rssi

            //BLUETOOTH_SCAN  은 API 레벨 31 부터 이므로 그이하는 ACCESS_FINE_LOCATION 로 대신한다.

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                if (ActivityCompat.checkSelfPermission(this@MainActivity, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                    Log.d("MainActivity", "BLUETOOTH_SCAN permission denied")
                    return
                }
            } else {
                if (ActivityCompat.checkSelfPermission(this@MainActivity, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                    Log.d("MainActivity", "ACCESS_FINE_LOCATION permission denied")
                    return
                }
            }

            Log.d("MainActivity", "Device found: ${device.name}, Address: ${device.address}, RSSI: $rssi")

            val existingDevice = devices.find {
                Log.d("MainActivity", "Checking address: ${it.address},  ${device.address}") // 로그 출력
                it.address == device.address
            }

            //이름이 있는것만 리스팅 한다.

            if(device.name == null) {
                return
            }


            val deviceName = device.name ?: "Unknown" // null이면 "Unknown"을 사용

            if( existingDevice != null) {
                existingDevice.rssi = rssi
                existingDevice.name = deviceName
                
            }
            else {
                val deviceItem = BluetoothDeviceItem(device.address, deviceName,rssi)
                devices.add(deviceItem)
            }
            adapter.notifyDataSetChanged()
        }

        override fun onScanFailed(errorCode: Int) {
            val errorMessage = when (errorCode) {
                ScanCallback.SCAN_FAILED_ALREADY_STARTED -> "Scan already started"
                ScanCallback.SCAN_FAILED_APPLICATION_REGISTRATION_FAILED -> "Application registration failed"
                ScanCallback.SCAN_FAILED_FEATURE_UNSUPPORTED -> "Feature unsupported"
                ScanCallback.SCAN_FAILED_INTERNAL_ERROR -> "Internal error"
                else -> "Unknown error code: $errorCode"
            }
            Log.e("MainActivity", "BLE scan failed: $errorMessage")
        }
    }

    private val bluetoothActivityResultLauncher = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
        if (result.resultCode == Activity.RESULT_OK) {
            // 블루투스가 활성화되었습니다.
            Log.d("MainActivity","bluetooth enabled")
        } else {
            // 사용자가 블루투스 활성화를 거부했습니다.
            Log.d("MainActivity","bluetooth denied")
        }
    }
}