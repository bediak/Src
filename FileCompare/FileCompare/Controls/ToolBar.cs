using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Media;

namespace FileCompare.Controls
{
    public class ToolBar : System.Windows.Controls.ToolBar
    {
        #region Fields

        private Dictionary<Control, Double> _overflowedControlsList = new Dictionary<Control, Double>();
        private ToolBarOverflowPanel _toolBarOverflowPanel;
        private ToggleButton _overflowToggleButton;
        private Boolean _suppressOverflowCorrections;
        private Boolean _overflowedControlWidthChanged;

        #endregion //Fields

        #region Properties

        public static readonly DependencyProperty OverflowPanelBackgroundProperty =
            DependencyProperty.Register("OverflowPanelBackground", typeof(Brush), typeof(ToolBar),
            new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.AffectsRender));

        public Brush OverflowPanelBackground
        {
            get { return (Brush)GetValue(OverflowPanelBackgroundProperty); }
            set { SetValue(OverflowPanelBackgroundProperty, value); }
        }

        public static readonly DependencyProperty OverflowToggleButtonBackgroundProperty =
            DependencyProperty.Register("OverflowToggleButtonBackground", typeof(Brush), typeof(ToolBar),
            new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.AffectsRender));

        public Brush OverflowToggleButtonBackground
        {
            get { return (Brush)GetValue(OverflowToggleButtonBackgroundProperty); }
            set { SetValue(OverflowToggleButtonBackgroundProperty, value); }
        }


        #endregion //Properties

        #region Public Methods

        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            if (OverflowPanelBackground == null)
            {
                OverflowPanelBackground = Background;
            }

            if (OverflowToggleButtonBackground == null)
            {
                OverflowToggleButtonBackground = Background;
            }

            if (_toolBarOverflowPanel == null)
            {
                _toolBarOverflowPanel = GetToolBarOverflowPanel();
                if (_toolBarOverflowPanel == null)
                {
                    return;
                }
                else
                {
                    _toolBarOverflowPanel.Loaded += ToolBarOverflowPanelOnLoadedHandler;
                }
            }

            if (_overflowToggleButton == null)
            {
                _overflowToggleButton = GetToggleButton();
                if (_overflowToggleButton == null)
                {
                    return;
                }
                else
                {
                    _overflowToggleButton.Loaded += OverflowToggleButtonOnLoadHandler;
                }
            }

            var overflowToggleButtonBorder = _overflowToggleButton.Parent as Border;
            if (overflowToggleButtonBorder != null)
            {
                overflowToggleButtonBorder.Background = OverflowToggleButtonBackground;
            }
            else
            {
                _overflowToggleButton.Background = OverflowToggleButtonBackground;
                _overflowToggleButton.Margin = new Thickness(0);
            }

            var overflowPanelBorder = _toolBarOverflowPanel.Parent as Border;
            if (overflowPanelBorder != null)
            {
                overflowPanelBorder.Background = OverflowPanelBackground;
            }
            else
            {
                _toolBarOverflowPanel.Background = OverflowPanelBackground;
                _toolBarOverflowPanel.Margin = new Thickness(0);
            }
        }

        #endregion //Public Methods

        #region Protected Methods

        protected override Size MeasureOverride(Size constraint)
        {
            if (_overflowedControlWidthChanged)
            {
                IsOverflowOpen = _suppressOverflowCorrections = true;
                foreach (var cachedItem in _overflowedControlsList)
                {
                    var control = cachedItem.Key;
                    var width = cachedItem.Value;
                    control.Width = width;
                }
                IsOverflowOpen = _suppressOverflowCorrections = false;
                _overflowedControlWidthChanged = false;
            }

            return base.MeasureOverride(constraint);
        }

        protected ToolBarOverflowPanel GetToolBarOverflowPanel()
        {
            return GetTemplateChild("PART_ToolBarOverflowPanel") as ToolBarOverflowPanel;
        }

        protected ToggleButton GetToggleButton()
        {
            return GetTemplateChild("OverflowButton") as ToggleButton;

        }

        protected ToolBarPanel GetToolBarPanel()
        {
            return GetTemplateChild("PART_ToolBarPanel") as ToolBarPanel;
        }

        #endregion //Protected Methods

        #region Events Handlers

        private void OverflowToggleButtonOnLoadHandler(Object sender, RoutedEventArgs e)
        {
            //TODO
            if (_suppressOverflowCorrections)
            {
                return;
            }
        }

        private void ToolBarOverflowPanelOnLoadedHandler(Object sender, RoutedEventArgs e)
        {
            if (_suppressOverflowCorrections)
            {
                return;
            }

            var width = GetMaxChildWidth(_toolBarOverflowPanel);
            if (width > 0)
            {
                foreach (var child in _toolBarOverflowPanel.Children)
                {
                    var control = child as Control;
                    if (control != null)
                    {
                        if (!_overflowedControlsList.ContainsKey(control))
                        {
                            _overflowedControlsList[control] = control.Width;
                        }
                        if (control.ActualWidth != width)
                        {
                            control.Width = width;
                            _overflowedControlWidthChanged = true;
                        }
                    }
                }
            }
        }

        #endregion //Events Handlers

        #region Private Methods

        private static Double GetMaxChildWidth(Panel panel)
        {
            var width = 0.0;
            foreach (var child in panel.Children)
            {
                var control = child as Control;
                if (control != null && control.ActualWidth > width)
                {
                    width = control.ActualWidth;
                }
            }
            return width;
        }

        #endregion //Private Methods
    }
}