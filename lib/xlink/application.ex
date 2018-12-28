defmodule Xlink.Application do
  use Application
  alias Xlink.{Downloader, Engine}

  @network_keys [
    "ifname", 
    "ssid", 
    "psk", 
    "key_mgmt", 
    "ipv4_address", 
    "ipv4_address_method",
    "ipv4_subnet_mask",
    "ipv4_broadcast",
    "ipv4_gateway"
  ]

  def start(_type, _args) do
    network_up()
    children = [
      {Downloader, []},
      {Engine, []}
    ]

    opts = [strategy: :one_for_one, name: Xlink.Supervisor]
    Supervisor.start_link(children, opts)
  end

  def network_up do
    networks = Application.get_env(:xlink, :config_file)
    |> File.read!()
    |> Jason.decode!()
    |> parse()
    old = Application.get_env(:nerves_network, :default, [])
    new = Enum.reduce(networks, old, fn({ifname, settings}, acc) ->
      Keyword.put(acc, String.to_atom(ifname), settings) 
    end)
    Application.put_env(:nerves_network, :default, new)

    for {ifname, settings} <- networks do
      Nerves.Network.setup(ifname, settings)
    end
    networks
  end

  def parse(list, acc \\ [])

  def parse([%{} = network | rest], acc) do
    {[ifname: ifname], settings} = 
      network
      |> Map.take(@network_keys)
      |> Enum.map(fn({key, value}) -> 
        case key do
          "ifname" -> {String.to_atom(key), value}
          "ssid" -> {String.to_atom(key), value}
          "psk" -> {String.to_atom(key), value}
          "ipv4" <> _ -> {String.to_atom(key), value}
          key -> {String.to_atom(key), String.to_atom(value)}
        end
      end)
      |> Keyword.split([:ifname])

    parse(rest, [{ifname, settings} | acc])
  end

  def parse([], acc), do: acc
end
