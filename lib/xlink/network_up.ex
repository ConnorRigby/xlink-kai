defmodule Xlink.NetworkUp do
  use GenServer
  require Logger
  alias Nerves.Network

  @doc "Are we connected to the internet?"
  def connected?(interface), do: GenServer.call(name(interface), :connected?)

  def not_connected(interface), do: GenServer.call(name(interface), :not_connected)

  @doc "Returns the current ip address"
  def ip_addr(interface), do: GenServer.call(name(interface), :ip_addr)

  def wait_for_dns(interface) do
    case connected?(interface) do
      true -> :ok
      _ ->
        Process.sleep(1500)
        wait_for_dns(interface)
    end
  end

  def dns_heartbeat(interface) do
    case connected?(interface) do
      true ->
        Process.sleep(15000)
        dns_heartbeat(interface)
      _ ->
        not_connected(interface)
        Process.sleep(1500)
        dns_heartbeat(interface)
    end
  end

  def name(interface), do: Module.concat(__MODULE__, interface)

  def start_link(interface, settings \\ %{}) do
    GenServer.start_link(__MODULE__, [interface, settings], [name: name(interface)])
  end

  def init([interface, settings]) do
    Network.setup(interface, settings)
    SystemRegistry.register()
    init_mdns(domain(interface))
    {:ok, %{connected: false, ip_address: nil, interface: interface, heartbeat: nil}}
  end

  def handle_info({:system_registry, :global, registry}, state) do
    ip = get_in registry, [:state, :network_interface, state.interface, :ipv4_address]
    if ip != state.ip_address do
      Logger.warn "IP ADDRESS CHANGED on #{state.interface}: #{ip}"
      update_mdns(ip, domain(state.interface))
    end

    connected = match?({:ok, {:hostent, _, [], :inet, 4, _}}, test_dns())
    if connected do
      heartbeat = spawn __MODULE__, :dns_heartbeat, [state.interface]
      {:noreply, %{state | ip_address: ip, connected: connected, heartbeat: heartbeat}}
    else
      {:noreply, %{state | ip_address: ip, connected: false}}
    end
  end

  def handle_info(_, state), do: {:noreply, state}

  def handle_call(:connected?, _from, state), do: {:reply, state.connected, state}
  def handle_call(:ip_addr, _from, state), do: {:reply, state.ip_address, state}
  def handle_call(:not_connected, _from, state) do
    Logger.warn "Disconnected!"
    if state.heartbeat && Process.alive?(state.heartbeat) do
      Process.exit(state.heartbeat, :kill)
    else
      Logger.error "Couldn't stop heartbeat process."
    end
    {:reply, :ok, %{state | connected: false, heartbeat: nil}}
  end

  def test_dns(hostname \\ 'teamxlink.co.uk') do
    :inet_res.gethostbyname(hostname)
  end

  defp domain(interface) do
    "xlink.#{interface}.local"
  end

  defp init_mdns(mdns_domain) do
    Mdns.Server.add_service(%Mdns.Server.Service{
      domain: mdns_domain,
      data: :ip,
      ttl: 120,
      type: :a
    })
  end

  defp update_mdns(ip, _mdns_domain) do
    ip_tuple = to_ip_tuple(ip)
    Mdns.Server.stop()

    # Give the interface time to settle to fix an issue where mDNS's multicast
    # membership is not registered. This occurs on wireless interfaces and
    # needs to be revisited.
    :timer.sleep(100)

    Mdns.Server.start(interface: ip_tuple)
    Mdns.Server.set_ip(ip_tuple)
  end

  defp to_ip_tuple(str) do
    str
    |> String.split(".")
    |> Enum.map(&String.to_integer/1)
    |> List.to_tuple()
  end
end
