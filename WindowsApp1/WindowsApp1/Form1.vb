Imports System.IO.Ports

Public Class Form1

    Dim connected As Boolean = False
    Dim tmp As String = ""
    Dim errcnt As Integer = 0
    Dim inbyte As Integer = 8
    Dim totalinfo As String = ""
    Dim sernum As Integer = 0

    Private Sub Button3_Click(sender As Object, e As EventArgs) Handles Button3.Click

        If TextBox7.Text = "ntutkenny" Then
            GroupBox1.Visible = True
        End If

    End Sub

    Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
        SerialPort1.Write(TextBox8.Text & vbCrLf &
                          TextBox2.Text & vbCrLf &
                          TextBox3.Text & vbCrLf &
                          TextBox4.Text & vbCrLf &
                          TextBox5.Text & vbCrLf &
                          TextBox6.Text)

        'Timer3.Enabled = True
    End Sub

    Private Sub Button4_Click(sender As Object, e As EventArgs) Handles Button4.Click
        SerialPort1.Write(TextBox8.Text & vbCrLf &
                          TextBox2.Text & vbCrLf &
                          TextBox3.Text & vbCrLf &
                          TextBox4.Text & vbCrLf &
                          TextBox5.Text & vbCrLf &
                          TextBox6.Text)
    End Sub

    Private Sub Timer2_Tick(sender As Object, e As EventArgs) Handles Timer2.Tick

        If SerialPort1.BytesToRead > 0 Then
            tmp = SerialPort1.ReadExisting()
            If tmp.IndexOf("end") >= 0 Then
                totalinfo &= tmp
            Else
                totalinfo &= tmp
            End If
        End If

        If totalinfo.IndexOf("end") >= 0 Then
            Dim a() As String = totalinfo.Split(vbLf)
            Dim aa As Integer
            For aa = 0 To a.Length - 1
                a(aa) = a(aa).Replace(vbCr, "")
                'Debug.Print(a(aa))
            Next aa

            TextBox8.Text = a(0)
            TextBox2.Text = a(1)
            TextBox3.Text = a(2)
            TextBox4.Text = a(3)
            TextBox5.Text = a(4)
            TextBox6.Text = a(5)

            Timer2.Enabled = False

        End If
    End Sub

    Private Sub Timer3_Tick(sender As Object, e As EventArgs) Handles Timer3.Tick

        If SerialPort1.BytesToRead > 0 Then
            tmp = SerialPort1.ReadExisting()
            Debug.Print(tmp)
            Timer3.Enabled = False
        End If

    End Sub

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load

        Dim serarr() As String = SerialPort.GetPortNames



        Dim i As Integer = 0

        For i = 0 To serarr.Length - 1
            SerialPort1.PortName = serarr(i)

            errcnt = 0
            SerialPort1.Parity = IO.Ports.Parity.None
            SerialPort1.StopBits = IO.Ports.StopBits.One
            SerialPort1.DataBits = 8
            SerialPort1.BaudRate = 9600

            Try
                connected = False
                totalinfo = ""
                inbyte = 8
                SerialPort1.Open()
            Catch ex As Exception
                Return
            End Try

            If SerialPort1.IsOpen() Then
                SerialPort1.Write("computer")
                Button1.Enabled = False

                While (SerialPort1.BytesToRead < 8 And errcnt < 3000)
                    errcnt += 1
                    Threading.Thread.Sleep(1)
                    Application.DoEvents()
                End While

                If SerialPort1.BytesToRead > 0 Then

                    tmp = SerialPort1.ReadExisting()

                    If tmp.IndexOf("connected") >= 0 Then
                        connected = True
                        Label9.Text = "已連線"
                        Button1.Visible = False
                        Timer1.Enabled = False
                        Timer2.Enabled = True
                        Button2.Enabled = True
                        Button3.Enabled = True
                        Button4.Enabled = True
                        Exit Sub
                    Else
                        If SerialPort1.IsOpen() = True Then
                            SerialPort1.Close()
                        End If
                    End If
                End If
            End If
        Next i

        MsgBox("請確定是否已連接", MsgBoxStyle.Critical, "Error")
        Me.Dispose()

    End Sub
End Class
