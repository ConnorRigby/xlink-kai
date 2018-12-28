defmodule Xlink.Engine do
  @moduledoc "Handles monitoring the engine."
  require Logger
  @dir Application.get_env(:xlink, :data_dir) || Mix.raise("no data dir")
  @exe Path.join(@dir, "kaiengine")

  use GenServer
  
  @doc false
  def start_link(args) do
    GenServer.start_link(__MODULE__, args, name: __MODULE__)

  end

  def init(_args) do
    SystemRegistry.register()
    File.mkdir_p("/root/.xlink")

    unless File.exists?("/root/.xlink/kaiUserConfig.txt") do
      Logger.info("Coppying default configuration.")

      File.cp(
        Path.join(:code.priv_dir(:xlink), "kaiUserConfig.txt"),
        "/root/.xlink/kaiUserConfig.txt"
      )
    end
    {:ok, nil}
  end

  def handle_info({:system_registry, :global, registry}, nil) do
    network_info = get_in(registry, [:state, :network_interface]) || []
    should_start? = Enum.any?(network_info, fn({ifname, info}) ->
      ip = is_binary(info[:ipv4_address])
      if ip do
        Logger.info("#{inspect(ifname)} => #{inspect(info[:ipv4_address])}")
        true
      end
    end)

    if should_start? do
      pid = spawn_link MuonTrap, :cmd, [@exe, [], [into: IO.stream(:stdio, :line)]]
      {:noreply, pid}
    else
      {:noreply, nil}
    end
  end

  def handle_info({:system_registry, :global, _}, pid) when is_pid(pid) do
    {:noreply, pid}
  end
end
