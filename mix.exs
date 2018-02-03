defmodule Xlink.MixProject do
  use Mix.Project

  @target System.get_env("MIX_TARGET") || "host"

  Mix.shell().info([
    :green,
    """
    Mix environment
      MIX_TARGET:   #{@target}
      MIX_ENV:      #{Mix.env()}
    """,
    :reset
  ])

  def project do
    [
      app: :xlink,
      version: "0.1.2",
      elixir: "~> 1.4",
      target: @target,
      archives: [nerves_bootstrap: "~> 0.6"],
      deps_path: "deps/#{@target}",
      build_path: "_build/#{@target}",
      lockfile: "mix.lock.#{@target}",
      build_embedded: Mix.env() == :prod,
      start_permanent: Mix.env() == :prod,
      aliases: aliases(@target),
      deps: deps()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application, do: application(@target)

  # Specify target specific application configurations
  # It is common that the application start function will start and supervise
  # applications which could cause the host to fail. Because of this, we only
  # invoke Xlink.start/2 when running on a target.
  def application("host") do
    [extra_applications: [:logger, :inets, :ssl]]
  end

  def application(_target) do
    [mod: {Xlink.Application, []}, extra_applications: [:logger, :inets, :ssl]]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:nerves, "~> 0.7", runtime: false},
      {:mdns, "~> 0.1"}
    ] ++ deps(@target)
  end

  # Specify target specific dependencies
  defp deps("host"), do: []

  defp deps(target) do
    [
      {:bootloader, "~> 0.1"},
      {:nerves_runtime, "~> 0.4"},
      {:nerves_network, "~> 0.3"},
      {:nerves_firmware_ssh, "~> 0.2"}
    ] ++ system(target)
  end

  defp system("rpi"), do: [{:nerves_system_rpi, ">= 0.0.0", runtime: false}]
  defp system("rpi0"), do: [{:nerves_system_rpi0, ">= 0.0.0", runtime: false}]
  defp system("rpi2"), do: [{:nerves_system_rpi2, ">= 0.0.0", runtime: false}]
  defp system("rpi3"), do: [{:nerves_system_rpi3, ">= 0.0.0", runtime: false}]
  defp system(target), do: Mix.raise("Unknown MIX_TARGET: #{target}")

  # We do not invoke the Nerves Env when running on the Host
  defp aliases("host"), do: []

  defp aliases(_target) do
    [
      "deps.precompile": ["nerves.precompile", "deps.precompile"],
      "deps.loadpaths": ["deps.loadpaths", "nerves.loadpaths"]
    ]
  end
end
