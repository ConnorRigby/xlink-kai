defmodule Xlink.Ntp do
  @moduledoc """
  Sets time.
  """

  import Xlink.NetworkUp, only: [test_dns: 1]
  require Logger

  @doc """
  Tries to set the time from ntp.
  This will try 3 times to set the time. if it fails the thrid time,
  it will return an error
  """
  def set_time(tries \\ 0)
  def set_time(tries) when tries < 4 do
    Process.sleep(1000 * tries)
    case test_dns('0.pool.ntp.org') do
      {:ok, {:hostent, _url, _, _, _, _}} ->
        do_try_set_time(tries)
      {:error, err} ->
        Logger.error "Failed to set time (#{tries}): DNS Lookup: #{inspect err}"
        set_time(tries + 1)
    end
  end

  def set_time(_), do: {:error, :timeout}

  defp do_try_set_time(tries) when tries < 4 do
    # we try to set ntp time 3 times before giving up.
    # Logger.busy 3, "Trying to set time (try #{tries})"
    :os.cmd('ntpd -q -p 0.pool.ntp.org -p 1.pool.ntp.org')
    wait_for_time(tries)
  end

  defp wait_for_time(tries, loops \\ 0)

  defp wait_for_time(tries, loops) when loops > 5 do
    :os.cmd('killall ntpd')
    set_time(tries)
  end

  defp wait_for_time(tries, loops) do
    case :os.system_time(:seconds) do
      t when t > 1_474_929 ->
        Logger.info "Time is set."
      _ ->
        Process.sleep(1_000 * loops)
        wait_for_time(tries, loops + 1)
    end
  end
end
